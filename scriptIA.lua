emu.speedmode("normal")

playStateAddress = 0x0068
gameStateAddress = 0x00C0

function isPlaying(gameState)
  return gameState == 4 and memory.readbyteunsigned(playStateAddress) < 9
end

local types_no_orientations = {
    'T', 'T', 'T', 'T',      -- 0x00 - 0x03
    'J', 'J', 'J', 'J',      -- 0x04 - 0x07
    'Z', 'Z',                -- 0x08 - 0x09
    'O',                     -- 0x0A
    'S', 'S',                -- 0x0B - 0x0C
    'L', 'L', 'L', 'L',      -- 0x0D - 0x10
    'I', 'I',                -- 0x11 - 0x12
    nil                      -- 0x13 (None)
}

function get_tetris_state()

    local actual_piece_address = 0x0042
    local next_piece_address = 0x00BF

    local actual_index = memory.readbyte(actual_piece_address)
    local next_index = memory.readbyte(next_piece_address)

    local actual = types_no_orientations[actual_index + 1] or "Unknown" -- +1 porque Lua es 1-indexed
    local next = types_no_orientations[next_index + 1] or "Unknown"

    local start_address = 0x0400
    local rows = 20
    local cols = 10
    local board = ""

    for row = 0, rows - 1 do
        local line = ""
        for col = 0, cols - 1 do
            local address = start_address + row * cols + col
            local cell_value = memory.readbyte(address)
            line = line .. (cell_value == 0xEF and "0" or "1")
        end
        board = board .. line
    end

    return actual, next, board
end

playStateAddress = 0x0048
tetriminoXAddress = 0x0060
tetriminoYAddress1 = 0x0061

function spawned()
  local playState = memory.readbyteunsigned(playStateAddress)
  local tetriminoX = memory.readbyteunsigned(tetriminoXAddress)
  local tetriminoY = memory.readbyteunsigned(tetriminoYAddress1)
  local result = lastPlayState ~= 1 and playState == 1 and tetriminoX == 5
      and tetriminoY == 0
  lastPlayState = playState
  return result
end

function generate_sequence(input_string)
    -- Imprime el string de entrada
    print(input_string)

    -- Elimina los paréntesis del string
    input_string = input_string:gsub("[()]", "")

    -- Divide el string en rotation y move
    local rotation, move = input_string:match("([^,]+),([^,]+)")
    rotation = tonumber(rotation)
    move = tonumber(move)

    -- Generar la secuencia de rotación
    local rotation_sequence = {}

    if rotation == 1 then
        rotation_sequence = {'A'}
    elseif rotation == 2 then
        rotation_sequence = {'A', 'NOOP', 'A'}
    elseif rotation == -1 then
        rotation_sequence = {'B'}
    elseif rotation == -2 then
        rotation_sequence = {'B', 'NOOP', 'B'}
    end

    -- Generar la secuencia de movimiento
    local move_sequence = {}
    if move >= 0 then
        for i = 1, move do
            table.insert(move_sequence, 'right')
            table.insert(move_sequence, 'NOOP')
        end
    else
        for i = 1, math.abs(move) do
            table.insert(move_sequence, 'left')
            table.insert(move_sequence, 'NOOP')
        end
    end

    -- Combinar las dos secuencias
    local sequence = {}
    for _, v in ipairs(rotation_sequence) do
        table.insert(sequence, v)
    end
    for _, v in ipairs(move_sequence) do
        table.insert(sequence, v)
    end

    table.insert(sequence, "NOOP")
    table.insert(sequence, "DOWN")

    return sequence
end

function send_input(action)
    local buttons = {}

    if action == "A" then
        buttons.A = true
    elseif action == "B" then
        buttons.B = true
    elseif action == "right" then
        buttons.right = true
    elseif action == "left" then
        buttons.left = true
    elseif action == "DOWN" then
        buttons.down = true
    elseif action == "NOOP" then
        buttons = {} -- No hace nada
    end

    joypad.set(1, buttons)
end

i = 0
sequence = {}

while true do
    local gameState = memory.readbyteunsigned(gameStateAddress)

    if isPlaying(gameState) then
        if spawned() then
            --print("Spauneo")
            local actual, next, board = get_tetris_state()

            --print("Actual piece: " .. actual)
            --print("Next piece: " .. next)
            --print("Board state:\n" .. board)

            -- Prepara los argumentos para el comando
            local command = string.format('./bestMove %s %s %s', actual, next, board)

            -- Ejecuta el comando con los valores concatenados
            local handle = io.popen(command)
            local output = handle:read('*a')
            local moves = output:gsub('[\n\r]', ' ')
            handle:close()

            -- Generar la secuencia a partir de la salida del comando
            sequence = generate_sequence(moves)
            --print(sequence)
            print('NIVEL ACTUAL :',memory.readbyteunsigned(0x0044))


            -- Reinicia el índice
            i = 0
        end

        -- Verifica si el índice está dentro de la secuencia
        if i <= #sequence then
            send_input(sequence[i])
            i = i + 1
        else
            -- Ejecuta el último movimiento repetidamente
            send_input("DOWN")
        end
    end
    gui.text(192, 190, "Nivel " .. memory.readbyteunsigned(0x0044) )
    emu.frameadvance() -- Mantiene el emulador avanzando
end

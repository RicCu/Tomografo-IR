function [ matrix ] = ReadMatrix( arduino , rows, cols)
%ReadMatrix Reads a float matrix sent through a serial connection.
%   Float values must be ASCII-encoded with a Terminator character between
%   each value in the matrix (this propetry must be set in the 'arduino')
%   object.
    value = fgetl(arduino);
    if value ~= 's'
        error('First value read was not start token "s"');
    end
    matrix = zeros(rows, cols);
    size(matrix)
    r = 1;
    c = cols;
    readVals = 1;
    while true
        %{
       while readVals
           value = fscanf(arduino, '%e');
           readVals = ~isfloat(value);
           disp(readVals)
       end
        %}
       value = fgetl(arduino);
       if value == 'e'
           break
       end
       value = str2double(value);
       c = mod(c, cols) + 1;
       disp(sprintf('row %i, col %i', r, c))
       disp(value)
       matrix(r, c) = value;
       
       if c == cols
           r = r + 1;
       end
    end


end


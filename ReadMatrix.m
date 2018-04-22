function [ matrix ] = ReadMatrix( arduino , rows, cols)
%ReadMatrix Reads a float matrix sent through a serial connection.
%   Float values must be ASCII-encoded with a Terminator character between
%   each value in the matrix (this propetry must be set in the 'arduino')
%   object.
    matrix = zeros(rows, cols);
    r = 1;
    c = cols;
    while arduino.BytesAvailable > 0
       value = fscanf(arduino, '%e');
       c = mod(c, cols) + 1;
       disp(sprintf('row %i, col %i', r, c))
       matrix(r, c) = value;
       
       if c == cols
           r = r + 1;
       end
    end


end


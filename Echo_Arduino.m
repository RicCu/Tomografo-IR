if ~isempty(instrfind)
     fclose(instrfind);
      delete(instrfind);
end
ard=serial('COM3','BaudRate',9600, 'Terminator', '/'); % create serial communication object 
fopen(ard); % initiate arduino communication
pause(3);   % Importante esperar a la conexion!!
fprintf(ard, '%c', 'm'); % Comando para mandar echo de matriz
while ard.BytesAvailable == 0 % Esperar respuesta
    pause(1);
end
disp('READING');
m = ReadMatrix(ard, 48, 1);
m
fclose(ard);


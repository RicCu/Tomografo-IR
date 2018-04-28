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
sinograma = ReadMatrix(ard, 48, 1)';
img = iradon(sinograma', 7.5);
subplot(1, 2, 1);

imshow(sinograma);
title('Sinograma', 'FontWeight', 'bold', 'FontSize', 16);
ylabel('Proyectores');
xlabel('Theta');
subplot(1, 2, 2);
imshow(img);
title('Reconstrucción', 'FontWeight', 'bold', 'FontSize', 16);

fclose(ard);


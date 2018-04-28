if ~isempty(instrfind)
     fclose(instrfind);
      delete(instrfind);
end
NUM_STEPS = 48;
NUM_DETECTORS = 3;
NUM_SLICES = 10;
ard=serial('COM3','BaudRate',9600, 'Terminator', '/'); % create serial communication object 
fopen(ard); % initiate arduino communication
pause(3); % Importante esperar a la conexion!!
%{
volume = zeros([NUM_STEPS, NUM_DETECTORS, NUM_SLICES]);
pause(3);   % Importante esperar a la conexion!!
fprintf(ard, '%c', 'm'); % Comando para mandar echo de matriz
pause(2);
%fprintf(ard, '%c', 'n'); % Comando para mandar echo de matriz
while ard.BytesAvailable == 0 % Esperar respuesta
    pause(1);
end
disp('READING');
%disp(fscanf(ard, '%e'));
%sinograms = [sinograms ReadMatrix(ard, 48, 3)];
volume(:,:,1) = ReadMatrix(ard, 48, 3);
%fprintf(ard, '%c', 'n'); % Comando para mandar echo de matriz

while ard.BytesAvailable == 0 % Esperar respuesta
    pause(1);
end
disp('READING');
%disp(fscanf(ard, '%e'));
%sinograms = [sinograms ReadMatrix(ard, 48, 3)];
volume(:,:,2) = ReadMatrix(ard, 48, 3);
while ard.BytesAvailable == 0 % Esperar respuesta
    pause(1);
end
disp('READING');
%disp(fscanf(ard, '%e'));
%sinograms = [sinograms ReadMatrix(ard, 48, 3)];
volume(:,:,3) = ReadMatrix(ard, 48, 3);
%}

vol = ReadVolume(ard, NUM_DETECTORS, NUM_STEPS, NUM_SLICES);

%{
img = iradon(sinograma', 7.5);
subplot(1, 2, 1);

imshow(sinograma);
title('Sinograma', 'FontWeight', 'bold', 'FontSize', 16);
ylabel('Proyectores');
xlabel('Theta');
subplot(1, 2, 2);
imshow(img);
title('Reconstrucción', 'FontWeight', 'bold', 'FontSize', 16);
%}
fclose(ard);


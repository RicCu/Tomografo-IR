function [ volume ] = ReadVolume( arduino, num_detectors, num_steps, num_slices )
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here

    volume = zeros([num_detectors, num_steps, num_slices]);
    fprintf(arduino, '%c', 'm'); % Comando para mandar echo de matriz
    pause(1);
    for i = 1:num_slices
        while arduino.BytesAvailable == 0 % Esperar respuesta
            pause(1);
        end
        disp('READING ');
        disp(i)
        volume(:,:,i) = ReadMatrix(arduino, num_steps, num_detectors)';
    end
end


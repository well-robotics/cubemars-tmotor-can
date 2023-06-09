classdef AkMotorLog < handle
    
    properties
        path = '/home/bucky/ROBOTLOG/AKmotors/'
 
     end
    
    methods % control
        function  obj = AkMotorLog()
        end

        function plot(obj)
            fileID = fopen( [obj.path, 'testing_Data']);
            raw = fread(fileID, 'float');
            data_names = {'ID','Pos', 'Vel','I', 'Temperature', 'time' };
            data_length = [1, 1, 1, 1, 1,1];
            N = floor(length(raw) / sum(data_length));  % Number of samples

             [ID, POS, VEL, Current,Temperature, ts] = obj.readRaw(raw, N, data_length);
       
            t = ts*1e-6;  % micro secs to s
            figure,
            subplot(3,1,1), plot(t, POS , 'r'); title('POS');
            diffPos = [1, diff(POS)];
            diffT = [1, diff(t)];
            subplot(3,1,2), plot(t, VEL , 'r', t, diffPos./diffT, 'b'); title('VEL');
            subplot(3,1,3), plot(t, Current , 'r'); title('Current');
        end
 
    end
 
    methods % 
        %%%  helper function to read the raw data file
        function  varargout  = readRaw(obj, raw, N, LengthVec)
            nout = max(nargout, 1) ;
            
            for i = 1:length(LengthVec)
                varargout{i} = zeros(LengthVec(i), N);
            end
            
            idx = 1;
            for i = 1:N
                for k = 1:nout
                    var_length = LengthVec(k);
                    varargout{k}(:,i) = raw(idx:idx + var_length -1);
                    idx = idx + var_length ;
                end
            end
            
        end
        
    end
end



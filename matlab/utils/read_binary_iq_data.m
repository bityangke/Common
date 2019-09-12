function [iq, i_data, q_data] = read_binary_iq_data(filename, data_type)

        % only supports the following data types: 'double', 'single',
        % u/int8, u/int16, u/int32, u/int64, float32, float64
        file_id = fopen(filename,'r');
        
        % get the file size
        file_start = ftell(file_id);
        fseek(file_id, 0, 'eof');
        file_stop = ftell(file_id);
        frewind(file_id);
        file_size = 0;
        
        % this sets the calculated files size for the expected data type
        if(strcmp(data_type, 'double') == 1)
            file_size = floor((file_stop - file_start)/(8*2));
        elseif(strcmp(data_type, 'single') == 1)
            file_size = floor((file_stop - file_start)/(4*2));
        elseif(contains(data_type, '64'))
            file_size = floor((file_stop - file_start)/(8*2));
        elseif(contains(data_type, '32'))
            file_size = floor((file_stop - file_start)/(4*2));
        elseif(contains(data_type, '16'))
            file_size = floor((file_stop - file_start)/(2*2));
        elseif(contains(data_type, '8'))
            file_size = floor((file_stop - file_start)/(2));
        end
        
        iq = fread(file_id, [2, file_size], data_type, 'ieee-le').';
        
        i_data = iq(:,1);
        q_data = iq(:,2);

        fclose(file_id);
    
end

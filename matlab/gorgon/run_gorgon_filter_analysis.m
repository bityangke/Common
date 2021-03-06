format long g
format compact
clc
close all
clearvars

% get the location of the script file to save figures
full_path = mfilename('fullpath');
[scriptpath,  filename, ext] = fileparts(full_path);
plot_num = 1;

%% get the folder to analyze
if(ispc)
    %start_path = 'D:/Projects/MNIST/results/';
    start_path = 'D:/IUPUI/PhD/Results/dfd_dnn/dnn_reduction';
elseif(isunix)
    start_path = '/home/owner/Projects/dfd_dnn_reduction';
else
    start_path = '';
end

filepath = uigetdir(start_path,'Select Folder');
if(filepath == 0)
    return;
end

listing = dir(filepath);
listing = listing(3:end);

commandwindow;

%%

file_id = fopen(fullfile(filepath,'gorgon_filter_analysis.txt'),'w');
fprintf('%s\n',filepath);
fprintf(file_id,'%s\n',filepath);

for idx=1:numel(listing)
    if(~listing(idx).isdir)
        continue;
    end
    
    xml_listing = dir(strcat(listing(idx).folder,filesep,listing(idx).name,filesep,'*.xml'));
    fprintf('Running gorgon analysis on %s\n',listing(idx).name);
    fprintf(file_id,'Running gorgon analysis on %s\n',listing(idx).name);
    tic;
    for jdx=1:numel(xml_listing)
        [layer, u] = gorgon_filter_analysis(fullfile(xml_listing(jdx).folder,xml_listing(jdx).name));
        fprintf('Layer: %03d\t Unique Classes: %03d\n', layer, numel(u));
        fprintf(file_id,'Layer: %03d\t Unique Classes: %03d\n', layer, numel(u));

        close all;
    end
    toc;
    fprintf('------------------------------------------------------\n');
    fprintf(file_id,'------------------------------------------------------\n');

end
fclose(file_id);
fprintf('Complete!\n');

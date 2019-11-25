format long g
format compact
clc
close all
clearvars

lib_path = 'D:/Projects/obj_det_lib/build/Release/';
lib_name = 'obj_det_lib';
hfile = 'D:/Projects/obj_det_lib/include/obj_det_lib.h';

if(~libisloaded(lib_name))
    [notfound, warnings] = loadlibrary(fullfile(lib_path, strcat(lib_name,'.dll')), hfile);
end

if(~libisloaded(lib_name))
   fprintf('\nThe %s library did not load correctly!',  lib_name);    
end

libfunctions(lib_name)
libfunctionsview(lib_name)

% void init_net(const char *net_name, unsigned int *num_classes, window* &det, unsigned int *num_win)
ld{idx,1} = libpointer('singlePtr', []);
ls{idx,1} = calllib(lib_name, lib_functions{idx}, layer_struct, ld{idx,1});
ld{idx,1}.reshape(ls{idx,1}.n,ls{idx,1}.size); 
calllib('obj_det_lib','init_net','D:/Projects/obj_det_lib/nets/yj_v4_s4_v4_BEAST_final_net.dat');

%% set up the graphs and locations

% [0.1440 0.2700 0.5090 0.6600]
f{1} = figure('Units', 'normalized', 'Position', [0.1440 0.2800 0.5090 0.6600], 'Name', 'Layer 12');
% f{1} = figure('Units', 'normalized', 'Position', [0.0900 0.5100 0.3500 0.4200], 'Name', 'Layer 12');
f{1}.ToolBar = 'none';

% f{2} = figure('Units', 'normalized', 'Position', [0.4400 0.5300 0.2800 0.4000], 'Name', 'Layer 09');
% f{2}.ToolBar = 'none';

% [0.6560 0.4465 0.3420 0.4843]
f{3} = figure('Units', 'normalized', 'Position', [0.6560 0.4565 0.3420 0.4843], 'Name', 'Layer 08');
% f{3} = figure('Units', 'normalized', 'Position', [0.7200 0.5300 0.2800 0.4000], 'Name', 'Layer 08');
f{3}.ToolBar = 'none';

% f{4} = figure('Units', 'normalized', 'Position', [0.0000 0.0400 0.3500 0.4100], 'Name', 'Layer 05');
% f{4}.ToolBar = 'none';

f{5} = figure('Units', 'normalized', 'Position', [0.3500 0.0400 0.3500 0.4100], 'Name', 'Layer 02');
f{5}.ToolBar = 'none';

% [0.6560 0.03705 0.3420 0.3550]
f{6} = figure('Units', 'normalized', 'Position', [0.6560 0.0370 0.3420 0.3550], 'Name', 'Layer 01');
% f{6} = figure('Units', 'normalized', 'Position', [0.7000 0.0400 0.2900 0.4100], 'Name', 'Layer 01');
f{6}.ToolBar = 'none';

% [0.0197916666666667 0.673148148148148 0.120833333333333 0.241666666666667] 
f{7} = figure('Units', 'normalized', 'Position', [0.0000 0.7000 0.1400 0.2400], 'Name', 'Input Image');
% f{7} = figure('Units', 'normalized', 'Position', [0.0000 0.7800 0.0900 0.1500], 'Name', 'Input Image');
f{7}.ToolBar = 'none';

f{8} = figure('Units', 'normalized', 'Position', [0.0000 0.4050 0.1400 0.2400], 'Name', 'Input Image');
% f{7} = figure('Units', 'normalized', 'Position', [0.0000 0.7800 0.0900 0.1500], 'Name', 'Input Image');
f{8}.ToolBar = 'none';

layer_struct = struct('k', 0, 'n',0, 'nr', 0, 'nc', 0, 'size',0);
%data = [];

%% get the camerra started

cam = webcam(1);
cam.Zoom = 200;
cam.Focus = 10;

% cam.Gain = 5;
% cam.Resolution = '320x180';

%% run the net
threshold = 128;
index = 0;

ld = cell(6,1);
ls = cell(6,1);

lib_functions = {'get_layer_12', 'get_layer_09', 'get_layer_08', 'get_layer_05', 'get_layer_02', 'get_layer_01'};

map_length = 1000;
cell_dim = [[7,19];[6,19];[6,19]];
padding = [4, 4, 2];


% get the image extents and assume they don't change during the demo
color_img = rot90(snapshot(cam),2);
img = rgb2gray(color_img);
[img_h, img_w] = size(img);
c = find(img(floor(img_h/2),:) > threshold);
r = find(img(:,floor(img_w/2)) > threshold);

c = (c(1)+2:c(end)-2);
r = (r(1)+2:r(end)-2);

img2 = 255 - (img(r, c));

img_s = double(imresize(img2, [28, 28]))';

min_img = min(img_s(:));
max_img = max(img_s(:));


while(true)

%tic
color_img = rot90(snapshot(cam),2);
img = rgb2gray(color_img);
% %img = rgb2gray(imread('D:\Projects\mnist\data\test\wc_test.png'));
% [img_h, img_w] = size(img);
% c = find(img(floor(img_h/2),:) > threshold);
% r = find(img(:,floor(img_w/2)) > threshold);
img2 = 255 - (img(r, c));

img_s = double(imresize(img2, [28, 28]))';

% min_img = min(img_s(:));
% max_img = max(img_s(:));

img_s = uint8(255*(img_s - min_img)/(max_img - min_img));

%img2 = rgb2gray(imread('D:\Projects\mnist\data\test\2_28.png'));
%img_s = img2';

[res] = calllib(lib_name, 'run_net', img_s(:), 28, 28);


figure(f{7});
image(color_img(r,c,:));
%colormap(gray(256));
axis('off');
ax = gca;
ax.Position = [0 0 1 1];


figure(f{8});
image(img_s');
colormap(gray(256));
axis('off');
ax = gca;
ax.Position = [0 0 1 1];

%% get the data


for idx=1:6
%     if ~libisloaded(lib_name)
%         loadlibrary(fullfile(lib_path, strcat(lib_name,'.dll')), hfile);
%     end
    ld{idx,1} = libpointer('singlePtr', []);
    ls{idx,1} = calllib(lib_name, lib_functions{idx}, layer_struct, ld{idx,1});
    ld{idx,1}.reshape(ls{idx,1}.n,ls{idx,1}.size);   
end



%% plot out the image sections


for idx=1:2:3
    
    layer_img = build_layer_image(ls{idx,1}, ld{idx,1}, cell_dim(idx,:), padding(idx), map_length);
    
    figure(f{idx});
    image(layer_img)
    axis('off');
    ax = gca;
    ax.Position = [0 0 1 1];
end

for idx=5:5
    figure(f{idx});
    bar([0:1:ls{idx,1}.k-1], ld{idx,1}.Value, 'FaceColor', 'b');
    box('on');
    grid('on');    
end

idx = 6;
figure(f{idx});
b1 = bar([0:1:ls{idx,1}.k-1], ld{idx,1}.Value, 'FaceColor', [0.5, 0.5, 0.5]);
hold('on');
b2 = bar(res, ld{idx,1}.Value(res+1), 'FaceColor','r');
box('on');
grid('on');
hold('off');




%% Layer 12
% ld_12 = libpointer('singlePtr', []);
% 
% [ls_12] = calllib(lib_name,'get_layer_12', layer_struct, ld_12);
% ld_12.reshape(ls_12.n,ls_12.size);
% 
% padding = 4;
% 
% %cell_dim = [7,19];
% 
% [layer_img_12] = build_layer_image(ls_12, ld_12, cell_dim, padding, map_length);
% 
% %f4 = figure('Units', 'normalized', 'Position', [0.1000 0.5000 0.3400 0.4200]);
% figure(f{2});
% image(layer_img_12)
% axis('off');
% ax = gca;
% ax.Position = [0 0 1 1];


%% Layer 09
% ld_09 = libpointer('singlePtr', []);
% 
% [ls_09] = calllib(lib_name,'get_layer_09', layer_struct, ld_09);
% ld_09.reshape(ls_09.n,ls_09.size);
% 
% padding = 4;
% map_length = 1000;
% cell_dim = [6,19];
% 
% [layer_img_09] = build_layer_image(ls_09, ld_09, cell_dim, padding, map_length);
% 
% %f5 = figure('Units', 'normalized', 'Position', [0.4400 0.5000 0.2800 0.4000]);
% figure(f{3});
% image(layer_img_09)
% axis('off');
% ax = gca;
% ax.Position = [0 0 1 1];

%% Layer 08
% ld_08 = libpointer('singlePtr', []);
% 
% [ls_08] = calllib(lib_name,'get_layer_08', layer_struct, ld_08);
% ld_08.reshape(ls_08.n,ls_08.size);
% 
% padding = 2;
% %map_length = 1000;
% cell_dim = [6,19];
% 
% [layer_img_08] = build_layer_image(ls_08, ld_08, cell_dim, padding, map_length);
% 
% %f6 = figure('Units', 'normalized', 'Position', [0.7200 0.5000 0.2800 0.4000]);
% figure(f{4});
% image(layer_img_08)
% axis('off');
% ax = gca;
% ax.Position = [0 0 1 1];

%% Layer 05

% ld_05 = libpointer('singlePtr', []);
% 
% [ls_05] = calllib(lib_name,'get_layer_05', layer_struct, ld_05);
% 
% ld_05.reshape(ls_05.n,ls_05.k);
% 
% %f3 = figure('Units', 'normalized', 'Position', [0.0000 0.0400 0.2900 0.4200]);
% figure(f{5});
% %hold on
% b1 = bar([0:1:ls_05.k-1], ld_05.Value, 'FaceColor', 'b');
% box('on');
% grid('on');

%% Layer 02

% ld_02 = libpointer('singlePtr', []);
% 
% [ls_02] = calllib(lib_name,'get_layer_02', layer_struct, ld_02);
% 
% ld_02.reshape(ls_02.n,ls_02.k);
% 
% %f2 = figure('Units', 'normalized', 'Position', [0.2900 0.0400 0.2900 0.4200]);
% figure(f{6});
% %hold on
% b1 = bar([0:1:ls_02.k-1], ld_02.Value, 'FaceColor', 'b');
% box('on');
% grid('on');

%% Layer 01

% ld_01 = libpointer('singlePtr', []);
% 
% [layer_struct1] = calllib(lib_name,'get_layer_01', layer_struct, ld_01);
% 
% ld_01.reshape(layer_struct1.n,layer_struct1.k);
% %ld_01.Value
% 
% %f1 = figure('Units', 'normalized', 'Position', [0.5800 0.0400 0.2900 0.4200]);
% 
% figure(f{7});
% b1 = bar([0:1:layer_struct1.k-1], ld_01.Value, 'FaceColor', [0.5, 0.5, 0.5]);
% hold('on');
% b2 = bar(res, ld_01.Value(res+1), 'FaceColor','r');
% box('on');
% grid('on');
% hold('off');
% %pause(0.5);

%toc
%index = index + 1;
end


%% close out the dll

% unloadlibrary(lib_name); 'MNIST_DLL'

% clear('cam');




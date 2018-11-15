format long g
format compact
clc
close all
clearvars


%% read in the data
dm_img = double(imread('D:\IUPUI\PhD\Results\dfd_dnn_pso\itr1\dfd_pso_13\depthmap_image_v6_pso_13_01_test_00049.png'));
dm_img_size = size(dm_img);

gt_img = double(imread('D:\IUPUI\Test_Data\rw\WS2\lidar\lidar_rng_right_00000_8bit.png'));
gt_img = gt_img(1:dm_img_size(1), 1:dm_img_size(2));
gt_img(gt_img>255) = 255;

% create a flat image based on the mean of the ground truth image
dm_flat = floor(mean(gt_img(:)))*ones(dm_img_size);

% display the images
img_sep = 255*ones(20, dm_img_size(2));

comb_img = cat(1,gt_img, img_sep, dm_img, img_sep, dm_flat);
figure;
image(comb_img);
colormap(gray(256));
axis off

%% calculate the metrics


sub_img = (dm_img - gt_img);
MAE(1) = mean(abs(sub_img(:)));
RMSE(1) = sqrt(mean(sub_img(:).^2));
SSIM(1) = ssim(gt_img, dm_img,'DynamicRange',255);
NMAE(1) = MAE(1)/255;
NRMSE(1) = RMSE(1)/255;

sub_img = (gt_img - dm_flat);
MAE(2) = mean(abs(sub_img(:)));
RMSE(2) = sqrt(mean(sub_img(:).^2));
SSIM(2) = ssim(gt_img, dm_flat,'DynamicRange',255);
NMAE(2) = MAE(2)/255;
NRMSE(2) = RMSE(2)/255;


NMAE
NRMSE
SSIM

%% high pass filter tests

%hpf = [0 -0.25 0; -0.25 2 -0.25; 0, -0.25, 0];
hpf = [1 1 1; 1 -8 1; 1 1 1];

gt_hpf = filter2(hpf, gt_img);

dm_hpf = filter2(hpf, dm_img);

flat_hpf = filter2(hpf, dm_flat);


dm_hpf_dif = abs(gt_hpf - dm_hpf);

mean(dm_hpf_dif(:))

flat_hpf_dif = abs(gt_hpf - flat_hpf);

mean(flat_hpf_dif(:))

figure; image(dm_hpf_dif); colormap(gray(256));
figure; image(flat_hpf_dif); colormap(gray(256));

sum_gt_hpf = sum(gt_hpf(:))
sum_dm_hpf = sum(dm_hpf(:))
sum_flat_hpf = sum(flat_hpf(:))


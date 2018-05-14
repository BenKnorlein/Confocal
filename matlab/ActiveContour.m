clear;
I0 = imread(['../tmp/image' '00000' '.tif']);
for i = 0:69
    count = sprintf('%05d',i);
    I = imread(['../tmp/image' count '.tif']) - I0;
   
    mask = imread(['../tmp/mask_circle' count '.tif']);
    bw = activecontour(I,mask,50,'edge');
    
    boundImg = uint16(bwperim(bw,4));
    boundImg(boundImg ~=0) = 256*256;
    imshow(I + boundImg)
end
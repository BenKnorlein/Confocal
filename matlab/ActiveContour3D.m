% clear;
% 
si = size(imread(['../tmp/image' '00000' '.tif']));

I = zeros(si(1), si(2), 70);
mask = zeros(si(1), si(2), 70);

for i = 0:69
    count = sprintf('%05d',i);
    I(:,:,i+1) = imread(['../tmp/image' count '.tif']);
    mask(:,:,i+1) = imread(['../tmp/mask_circle' count '.tif']);
end
% 
% bw = activecontour(I,mask,300);
% 
% figure;
% p = patch(isosurface(double(bw)));
% p.FaceColor = 'red';
% p.EdgeColor = 'none';
% daspect([1.0/0.645 1.0/0.645 1.0/5]);
% camlight; 
% lighting phong

bw = bw_backup;

% Remove smaller segmented regions and only keep the largest one
CC = bwconncomp(bw,double(18));
numPixels = squeeze(cellfun(@numel,CC.PixelIdxList));
max_pixel = max(numPixels);
for i = 1:size(numPixels,2)
    if numPixels(i) ~= max_pixel
        idx_delete = CC.PixelIdxList{i};
        bw(idx_delete) = 0;
    end
end

% remove the last layer
slice_sum = squeeze(sum(sum(bw)));
slice_max = find(max(slice_sum) == slice_sum);
if slice_max < size(bw,3)
    bw(:,:,slice_max+1:size(bw,3)) = 0;
end

%compute isosurface
surface = isosurface(double(bw));
surface_backup = surface;

%remove backfacing polygons
idx = []
size(surface.faces,1)
size(surface.vertices,1)
for i = 1:size(surface.faces,1)
    if surface.vertices(surface.faces(i,1),3) > slice_max & ...
        surface.vertices(surface.faces(i,2),3) > slice_max & ...
        surface.vertices(surface.faces(i,3),3) > slice_max
    idx = [idx , i];
    end   
end
surface.faces(idx,:) = [];

removeVerticesPatch(surface,[]);
size(surface.faces,1)
size(surface.vertices,1)

figure;
p = patch(surface);
p.FaceColor = 'red';
p.EdgeColor = 'none';
daspect([1.0/0.645 1.0/0.645 1.0/5]);
camlight; 
lighting phong

% [X,Y,Z] = meshgrid(0:0.645:0.645*(size(bw,1)-1), 0:0.645:0.645*(size(bw,2)-1),0:5:5*(size(bw,3)-1));
% [F,V] = MarchingCubes(X,Y,Z,bw,0.5);

bw_filled = bw;
for i = 1:size(bw,3)
   bw_filled(:,:,i) = imfill(squeeze(bw(:,:,i)),'holes');
end

bw_inverted = imcomplement(bw_filled);
if slice_max < size(bw_inverted,3)
    bw_inverted(:,:,slice_max+1:size(bw_inverted,3)) = 0;
end
bw_dist = bwdistsc(bw_inverted,[0.645 0.645 5]);

bw_dist = bw_dist .* bw;
for i = 1:size(bw_dist,3)
   imshow(squeeze(bw_dist(:,:,i)),[]) ;
   waitforbuttonpress
end


colors = ['y' 'm' 'c' 'r' 'g' 'b'];
val = max(max(max(B)));
nbSlices = 5;
figure;
hold on

daspect([1.0/0.645 1.0/0.645 1.0/5]);
camlight('left'); 
camlight('right'); 
camlight;
lighting phong
    
for m = 0:nbSlices - 1
    surface = isosurface(double(B), val/nbSlices * m);
    slice_max = 33;
    %remove backfacing polygons
    idx = [];
    for i = 1:size(surface.faces,1)
        if surface.vertices(surface.faces(i,1),3) > slice_max & ...
            surface.vertices(surface.faces(i,2),3) > slice_max & ...
            surface.vertices(surface.faces(i,3),3) > slice_max
        idx = [idx , i];
        end   
    end
    surface.faces(idx,:) = [];

    %remove unused vertices
    removeVerticesPatch(surface,[]);
    
    p = patch(surface);
    p.FaceColor = colors(mod(m,size(colors,2)) + 1);
    p.EdgeColor = 'none';
    drawnow
end
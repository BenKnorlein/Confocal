function [Surface, DistanceMap] = processActiveContour(ImageStack, Mask, Params, Aspect)
%PROCESSACTIVECONTOUR Summary of this function goes here
% [A , B] = processActiveContour(I,mask,[300 0 18],[0.645 0.645 5])

nbIterations = Params(1);
draw = Params(2);
con_neighbourhood = double(Params(3));

%compute countour
bw = activecontour(ImageStack,Mask,nbIterations);

% Remove smaller segmented regions and only keep the largest one
CC = bwconncomp(bw, con_neighbourhood);
num_pixels = squeeze(cellfun(@numel,CC.PixelIdxList));
max_pixel = max(num_pixels);
for i = 1:size(num_pixels,2)
    if num_pixels(i) ~= max_pixel
        idx_delete = CC.PixelIdxList{i};
        bw(idx_delete) = 0;
    end
end
   
% remove the layers after the layer containing the most pixel
slice_sum = squeeze(sum(sum(bw)));
slice_max = find(max(slice_sum) == slice_sum);
if slice_max < size(bw,3)
    bw(:,:,slice_max+1:size(bw,3)) = 0;
end

% Repeat : remove smaller segmented regions and only keep the largest one
% as some might not be connected anymore
CC = bwconncomp(bw, con_neighbourhood);
num_pixels = squeeze(cellfun(@numel,CC.PixelIdxList));
max_pixel = max(num_pixels);
for i = 1:size(num_pixels,2)
    if num_pixels(i) ~= max_pixel
        idx_delete = CC.PixelIdxList{i};
        bw(idx_delete) = 0;
    end
end

%compute isosurface
surface = isosurface(double(bw));
 
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

%remove not connected polygons
edges = zeros(2,size(surface.faces,1)*3);
for i = 1:size(surface.faces,1)
   edges(1,(i-1)*3 + 1) = surface.faces(i,1);
   edges(1,(i-1)*3 + 2) = surface.faces(i,2);
   edges(1,(i-1)*3 + 3) = surface.faces(i,1);
   
   edges(2,(i-1)*3 + 1) = surface.faces(i,2);
   edges(2,(i-1)*3 + 2) = surface.faces(i,3);
   edges(2,(i-1)*3 + 3) = surface.faces(i,3);
end
G = graph(edges(1,:),edges(2,:));
SG = adjacency(G);
[S,C] = graphconncomp(SG);
a = histcounts(C,max(max(C)));
[m g_idx] = max(a) ;
idx = [];
for i = 1:size(surface.faces,1)
    if C(surface.faces(i,1)) ~=  g_idx | ...
       C(surface.faces(i,2)) ~=  g_idx | ...
       C(surface.faces(i,3)) ~=  g_idx
        idx = [idx , i];
    end   
end
surface.faces(idx,:) = [];

%remove unused vertices
removeVerticesPatch(surface,[]);

% fill holes - removed as it is done in inverted map
%bw_filled = bw;
%for i = 1:size(bw,3)
%   bw_filled(:,:,i) = imfill(squeeze(bw(:,:,i)),'holes');
%end

% invert and remove back
bw_inverted = imcomplement(bw);
if slice_max < size(bw_inverted,3)
    bw_inverted(:,:,slice_max+1:size(bw_inverted,3)) = 0;
end
%fill holes not connected to background
CC = bwconncomp(bw_inverted, 26);
num_pixels = squeeze(cellfun(@numel,CC.PixelIdxList));
max_pixel = max(num_pixels);
for i = 1:size(num_pixels,2)
    if num_pixels(i) ~= max_pixel
        idx_delete = CC.PixelIdxList{i};
        bw_inverted(idx_delete) = 0;
        bw(idx_delete) = 1;
    end
end

%compute distance filled pixels
bw_dist = bwdistsc(bw_inverted,Aspect);

% combine with original to remove backside
bw_dist = bw_dist .* bw;

% draw isosurface
if draw > 0
    figure;
    p = patch(surface);
    p.FaceColor = 'red';
    p.EdgeColor = 'none';
    daspect([1.0/Aspect(1) 1.0/Aspect(2) 1.0/Aspect(3)]);
    camlight; 
    lighting phong
end

% return parameters
Surface = surface;
DistanceMap = double(bw_dist);
end


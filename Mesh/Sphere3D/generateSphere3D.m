close all 
clear
clc

% ------------------------------------------------------------------------
% --- DO MODIFY HERE, AND ONLY HERE ---

it = 1; % number of layers of the sphere
plotIt = 0; % 0 not to show the plots, 1 to show them

% ------------------------------------------------------------------------
% --- DO NOT MODIFY BELOW THIS LINE! ---
% ------------------------------------------------------------------------

it = it+1;
xx = [];
yy = [];
zz = [];

r = linspace(0,1,it);
theta = linspace(0,2*pi,2*it);
phi = linspace(0,pi/2,it);
r = r(2:end); % to avoid repeating many times the center node (will be added manually later)
theta = theta(1:end-1); % to avoid doubling the 0=2pi points
phi = phi(1:end-1); % to avoid having many times the poles (will be added manually later)
[theta,phi] = meshgrid(theta,phi);

if (plotIt == 1)
    hold on
end

for radius = r
    [xs,ys,zs] = sph2cart(theta,phi,radius);
    %if (plotIt == 1)
    %    surf(xs,ys,zs)
    %    surf(xs(end:-1:2,:), ys(end:-1:2,:), -zs(end:-1:2,:))
    %end

    xx = [xx; xs; xs(end:-1:2,:)];
    yy = [yy; ys; ys(end:-1:2,:)];
    zz = [zz; zs; -zs(end:-1:2,:)];
end

nel = numel(xx);
xx = reshape(xx, nel, 1); xx = [0; xx; 0; 0];
yy = reshape(yy, nel, 1); yy = [0; yy; 0; 0];
zz = reshape(zz, nel, 1); zz = [0; zz; 1; -1];
M = [xx yy zz];

TRI = delaunayn(M);
if (plotIt == 1)
    tetramesh(TRI,M);
end

% print it to files
csvwrite(['point-',num2str(it-1),'.txt'],M);

outConnFile = ['conn-',num2str(it-1),'.txt'];
fileID = fopen(outConnFile,'w');
for row = 1:size(TRI,1)
    fprintf(fileID,'%i,%i,%i;',TRI(row,1),TRI(row,2),TRI(row,3));
    fprintf(fileID,'%i,%i,%i;',TRI(row,1),TRI(row,2),TRI(row,4));
    fprintf(fileID,'%i,%i,%i;',TRI(row,1),TRI(row,3),TRI(row,4));
    fprintf(fileID,'%i,%i,%i\n',TRI(row,4),TRI(row,2),TRI(row,3));
end
fclose(fileID);


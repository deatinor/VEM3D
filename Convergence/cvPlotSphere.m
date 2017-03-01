% Analyse convergence of Laplace equation with Dirichlet boundary
% conditions with the linear (k=1) virtual element method
%
% Geometry: 3D -> full sphere \Omega centered in (0,0,0) and of radius 1.
%
% Equation: -lap(u)=6 on \Omega; u=1-r^2 on \partial\Omega
%

clear
close all
clc

% Data
h = csvread('MeshSphere3D/hTriangle.txt');

% Load error from file
% Each row correspond to a given number of elements (from 2^0 to 2^7),
% the first column is the error in the L^\inf-norm, the 2nd one is the 
% error in the H^1-norm.

err3d = csvread('OutputSphere/error3D.txt');

%% Analysis
% Compute analytically the error convergence slope
dh1=err3d(2:end-1,2)./err3d(3:end,2);
dli=err3d(2:end-1,1)./err3d(3:end,1);
dh=h(2:end-1)./h(3:end);

convH=log(dh1)./log(dh)
convL=log(dli)./log(dh)

% Visualization of the error convergence
exact = h(2:end);
exact = exact*0.5;
figure;
loglog(h,err3d(:,2),'+-b');
hold on;
loglog(h(2:end),exact,'--k');
grid on;
xlabel('Size of the elements h');
ylabel('H^1-error');
xlim([h(end)-0.001,h(1)+0.1]);
title('Plot of the error in function of the element size');
legend('Error VEM', 'h', 'Location', 'northwest');


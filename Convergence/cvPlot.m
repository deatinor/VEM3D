% Analyse convergence of Laplace equation with Dirichlet boundary
% conditions with the linear (k=1) virtual element method
%
% Geometry: 2D -> square \Omega = (0,1)x(0.1) divided into n square 
% elements, with n ranging from 2^0=1 to 2^7=128
%
% Equation: -lap(u)=0 on \Omega; u=f on \partial\Omega
%

clear
close all
clc

% Data
ne = 2.^(1:7); % number of elements
h = 1./ne; % size of the elements

% Load error from file
% Each row correspond to a given number of elements (from 2^0 to 2^7),
% the first column is the error in the L^\inf-norm, the 2nd one is the 
% error in the H^1-norm.

% err1d = csvread(Output/error1D.csv');
err2d = csvread('Output/error2D.txt');
err3d = csvread('Output/error3D.txt');

%% 2D analysis
% Compute analytically the error convergence slope
dh1=err2d(4:end-1,2)./err2d(5:end,2);
dli=err2d(4:end-1,1)./err2d(5:end,1);
dh=h(4:end-1)./h(5:end);

convH=log(dh1)./log(dh)'
convL=log(dli)./log(dh)'

% Visualization of the error convergence
exact = h(4:end).^2;
exact = exact*10^3;
figure;
loglog(h,err2d(:,2),'+-b');
hold on;
loglog(h(4:end),exact,'--k');
grid on;
xlabel('Size of the elements h');
ylabel('H^1-error');
xlim([h(end)-0.001,h(1)+0.1]);
title('Plot of the error in function of the element size');
legend('Error VEM', 'h^2', 'Location', 'northwest');

%% 3D analysis
% Compute analytically the error convergence slope
dh1=err3d(4:end-1,2)./err3d(5:end,2);
dli=err3d(4:end-1,1)./err3d(5:end,1);
dh=h(4:end-1)./h(5:end);

convH=log(dh1)./log(dh)'
convL=log(dli)./log(dh)'

% Visualization of the error convergence
exact = h(5:end).^2;
exact = exact*10^4;
figure;
loglog(h(1:end),err3d(:,2),'+-b');
hold on;
loglog(h(5:end),exact,'--k');
grid on;
xlabel('Size of the elements h');
ylabel('H^1-error');
xlim([h(end)-0.001,h(1)+0.1]);
title('Plot of the error in function of the element size');
legend('Error VEM', 'h^2', 'Location', 'northwest');


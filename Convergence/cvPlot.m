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

figure;
loglog(h,err2d(:,2),'+-');
grid on;
xlabel('Size of the elements h');
ylabel('H^1-error');
title('Plot of the error in function of the element size');


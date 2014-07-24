function [h,u,v,rnorm] = pdcoLSprimal( Q,h0 )

%        [h,u,v,rnorm] = pdcoLSprimal( Q,h0 );
% solves the constrained least squares problem
%
%   min ||h-h0||^2  st  Q*h >= 0
%
% by applying PDCO to the regularized primal problem
%
%   min 1/2 ||h-h0||^2 + 1/2 ||d1*h||^2 + 1/2 ||d1*s||^2 + 1/2 ||r||^2
%  h,s,r
%   st        Q*h - s + d2*r >= 0,  s >= 0,
%
% where d1 and d2 are both rather small, say 1e-3, but not too small.
%
% Output:
%   h      primal variables
%   u      dual variables (Lagrange multipliers for Qh >= 0)
%   v      Q*h
%   rnorm  residual norm ||h-h0||

%-----------------------------------------------------------------------
% 20 Feb 2014: Primal and dual problem described by
%              Ilya Palachev <iliyapalachev@gmail.com>
%              Moscow State University.
%              https://bitbucket.org/mexmat_polyhedral_algorithms/polyhedra-correction-library/wiki/Home
% 24 Feb 2014: Data Q, h0 provided by Ilya.
%              pdcoLSprimal.m derived from pdcotestLP.m.
%              Michael Saunders <saunders@stanford.edu>
%              SOL, Stanford University.
%              http://www.stanford.edu/group/SOL/
%-----------------------------------------------------------------------

  [mQ,nQ] = size(Q);
  m       = mQ;
  n       = nQ + m;
  emQ     =  ones(mQ,1);
  enQ     =  ones(nQ,1);
  zmQ     = zeros(mQ,1);
  znQ     = zeros(nQ,1);

  PDobj   = @(x) LSobj(x,mQ,nQ,h0 );
  A       = [Q -speye(m)];
  b       = zmQ;
  bl      = [-inf(nQ,1); zmQ];
  bu      = inf(n,1);
  s0      = emQ;

  d1      = 1e-3;    % Not too small
  d2      = 1e-4;    % Not too small

  options = pdcoSet;

  x0      = [h0; s0];        % Initial h, s
  y0      =  zmQ;            % Initial y
  z0      = [znQ; emQ];      % Initial z
  xsize   = 1;               % Estimate of norm(x,inf) at solution
  zsize   = 1/d2;            % Estimate of norm(z,inf) at solution

  options.MaxIter   = 100;
  options.mu0       = 1e-0;  % An absolute value
  options.Method    = 1;     % Sparse Cholesky
  options.wait      = 1;     % Allow options to be reviewed before solve

  [x,y,z,inform,PDitns,CGitns,time] = ...
    pdco( PDobj,A,b,bl,bu,d1,d2,options,x0,y0,z0,xsize,zsize );

  h       = x(1:nQ);
  u       = y;
  v       = Q*h;
  rnorm   = norm(h-h0);
  fprintf('\n')
  fprintf('||h0||, ||h-h0|| %15.5f   %15.5f\n',norm(h0),rnorm  )
  fprintf('max(h0),min(h0)  %15.5f   %15.5f\n', max(h0),min(h0))
  fprintf('max(h), min(h)   %15.5f   %15.5f\n', max(h), min(h) )
  fprintf('max(u), min(u)   %15.5f   %15.5f\n', max(u), min(u) )
  fprintf('max(v), min(v)   %15.5f   %15.5f\n', max(v), min(v) )

  keyboard                   % Allow review of h,u,v, etc.
%-----------------------------------------------------------------------
% End function pdcoLSprimal
%-----------------------------------------------------------------------

function [obj,grad,hess] = LSobj( x,mQ,nQ,h0 )
%        [obj,grad,hess] = LSobj( x,mQ,nQ,h0 );
%        computes the objective value, gradient and diagonal Hessian
%        of the function  1/2||h-h0||^2 for use with pdco.m.
%        x = [h; s].

%-----------------------------------------------------------------------
% 24 Feb 2014: LSobj returns a diagonal Hessian.
%              obj  = 1/2||h-h0||^2
%              grad = [   h-h0   ; zeros(mQ,1)]
%              hess = [ones(nQ,1); zeros(mQ,1)]
%-----------------------------------------------------------------------
   h    = x(1:nQ);
   dh   = h - h0;

   enQ  =  ones(nQ,1);
   zmQ  = zeros(mQ,1);

   obj  = 0.5*norm(dh)^2;
   grad = [ dh; zmQ];
   hess = [enQ; zmQ];

%-----------------------------------------------------------------------
% End function LSobj
%-----------------------------------------------------------------------

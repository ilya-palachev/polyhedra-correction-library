function [h,u,v,rnorm] = pdcoLSdual( Q,h0 )

%        [h,u,v,rnorm] = pdcoLSdual( Q,h0 );
% solves the constrained least squares problem
%
%    min ||h-h0||^2    st   Q*h >= 0
%
% by noting its dual problem
%
%    min ||Q'*u - h0||^2  st  u >= 0
%
% and applying PDCO to the regularized dual problem
%
%    min 1/2 ||d1*u||^2 + 1/2 ||r||^2
%    u,r
%    st        Q'*u  +  r = h0,
%
% where d1 is rather small, say 1e-5, but not too small.
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
%              pdcoLSdual.m   derived from pdcoLSprimal.m.
%              Michael Saunders <saunders@stanford.edu>
%              SOL, Stanford University.
%              http://www.stanford.edu/group/SOL/
%-----------------------------------------------------------------------

  [mQ,nQ] = size(Q);
  m       = nQ;
  n       = mQ;
  emQ     =  ones(mQ,1);
  enQ     =  ones(nQ,1);
  zmQ     = zeros(mQ,1);
  znQ     = zeros(nQ,1);

  c       = zmQ;
  A       = Q';   % m x n
  b       = h0;
  bl      = zmQ;
  bu      = inf(mQ,1);

  d1      = 1e-5;    % Not too small
  d2      = 1;       % Least squares

  options = pdcoSet;
  
  x0      = emQ;             % initial u
  y0      = h0;              % Initial y
  z0      = emQ;             % Initial z
  xsize   = 1/d1;            % Estimate of norm(x,inf) at solution
  zsize   = 1;               % Estimate of norm(z,inf) at solution

  options.MaxIter   = 100;
  options.mu0       = 1e-0;  % An absolute value
  options.Method    = 1;     % Sparse Cholesky
  options.wait      = 1;     % Allow options to be reviewed before solve

  [x,y,z,inform,PDitns,CGitns,time] = ...
    pdco( c,A,b,bl,bu,d1,d2,options,x0,y0,z0,xsize,zsize );

  u       = x;
  h       = y;
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
% End function pdcoLSdual
%-----------------------------------------------------------------------

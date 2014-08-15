% 24 Feb 2014: Script to load example data from Ilya.
%              Lines 1323, 7602, 10727, 11834, 12190, 13498, 13682, 13785, 15566, 15588
%              are missing the last (j,value) pair.
%              Copied value from the next line in each case.


h0 = load('hvalues-naive.txt'); % contains h0         5237 vector
M  = load('matrix-naive.txt');  % contains Q  15705 x 5237 matrix

[mQ nc] = size(M);  % mQ = 15705
                    % nc = 9 cols of [i   j1 v1   j2 v2   j3 v3   j4 v4]

em      = (1:mQ)';
I       = [em;         em;     em;     em];
J       = [M(:,2); M(:,4); M(:,6); M(:,8)] + 1;   % Allow for 0 base
V       = [M(:,3); M(:,5); M(:,7); M(:,9)];

Q       = sparse(I,J,V);

format compact
axes('Position',[0.2 0.2 0.6 0.6]);
vert = [-1 -1 -1;  1 -1 -1;  1  1 -1; -1  1 -1;
         -1 -1  1;  1 -1  1;  1.25  1.25  1.25; -1  1  1];
fac = [1 2 3 4; ...
    5 6 8 8; ...
    1 2 6 5; ...
    2 3 6 6; ...
    3 4 8 8; ...
    4 1 5 8; ...
    6 7 8 8; ...
    3 7 6 6; ...
    8 7 3 3];
patch('Faces',fac,'Vertices',vert,'FaceColor','r', 'Marker', 'o','MarkerSize', 7, 'MarkerFaceColor', 'b');
material shiny;
alpha(0.8);
alphamap('rampdown');
view(30,30);
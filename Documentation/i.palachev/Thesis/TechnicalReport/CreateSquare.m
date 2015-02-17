plot([1,3,3,1,1],[1,1,-1,-1,1], 'linewidth', 10); hold on
grid on
plot([0,4],[0,0],'Color',[0 0 0])
plot([0,0],[-2,2],'k:')
set(gca,'ytick',[-2:0.5:2])
set(gca,'xtick',[0:0.5:4]); hold off

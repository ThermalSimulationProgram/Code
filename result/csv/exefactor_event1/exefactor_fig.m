
prefix = 'exefactor';
postfix = '_result.csv';

exefactors = 0.1 : 0.1 : 1;

aptmResults = [];
bwsResults  = [];
pbooResults = [];



kernels = {'aptm', 'bws', 'pboo'};
control = [1, 1, 1];
for p = exefactors
    
    index = 1;
    for k = kernels
        if control(index)
            file = [prefix, num2str(p,'%0.1f'), '_', k{1}, postfix];
            r = getResultFromCsv(file);
            switch index
                case 1
                    aptmResults = [aptmResults, r];
                case 2
                    bwsResults  = [bwsResults, r];
                case 3
                    pbooResults = [pbooResults, r];
                otherwise
            end
        end
        
        
        index = index + 1;
        
    end 
end



numberSamples = numel(aptmResults);

cel2K = 0;

avrgT_aptm = zeros(1, numberSamples) ;
avrgT_bws  = avrgT_aptm;
avrgT_pboo = avrgT_aptm;

for i = 1 : numberSamples
    avrgT_aptm(i) = aptmResults(i).avrgT + cel2K;
    avrgT_bws(i) = bwsResults(i).avrgT + cel2K;
    avrgT_pboo(i) = pbooResults(i).avrgT + cel2K;
end

plot(exefactors, avrgT_aptm, 'r-d', exefactors, avrgT_bws, 'b-*', exefactors, ...
    avrgT_pboo,'g-s','linewidth', 1.5, 'MarkerSize',10);


fsize = 13;
%% post processing
% set(gca, 'xlim',[min(exefactors)-0.1, max(exefactors)+ 0.1],'xtick', min(exefactors):0.1:max(exefactors),...
%     'ylim',[310 340],'FontSize',fsize,...
%     'FontName','Times New Roman','Position',[0.13 0.15 0.8 0.6]);
%  xlabel('execution factor','FontSize',fsize)
%  ylabel('Temperature / K','FontSize',fsize, 'Rotation', 90)
% box on;
% 
% 
% %% legend
% h=legend('APTM','BWS','O-PBOO', 'Location', 'Best');
% pos = get(h,'pos');
% % pos(1) = pos(1) - 0.5;
% % pos(2) = pos(2) + 0.3;
% set(h, 'FontName','Times New Roman','fontsize',fsize ,'box','on');
% grid on
% 
% % saveas(gca, 'deadline_dell9020.eps' ,'epsc2') ;
% 


prefix = 'deadline';
postfix = '_result.csv';

periods = 700 : 20 : 940;

aptmResults = [];
bwsResults  = [];
pbooResults = [];



kernels = {'aptm', 'bws', 'pboo'};
control = [1, 1, 1];
for p = periods
    
    index = 1;
    for k = kernels
        if control(index)
            file = [prefix, num2str(p), '_', k{1}, postfix];
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

cel2K = 273.15;

avrgT_aptm = zeros(1, numberSamples) ;
avrgT_bws  = avrgT_aptm;
avrgT_pboo = avrgT_aptm;

for i = 1 : numberSamples
    avrgT_aptm(i) = aptmResults(i).avrgT + cel2K;
    avrgT_bws(i) = bwsResults(i).avrgT + cel2K;
    avrgT_pboo(i) = pbooResults(i).avrgT + cel2K;
end

plot(periods, avrgT_aptm, 'r-d', periods, avrgT_bws, 'b-*', periods, ...
    avrgT_pboo,'g-s','linewidth', 1.5, 'MarkerSize',10);


fsize = 13;
%% post processing
set(gca, 'xlim',[min(periods)-10, max(periods)+ 10],'xtick', min(periods):60:max(periods),...
    'ylim',[310 345],'FontSize',fsize,...
    'FontName','Times New Roman','Position',[0.13 0.15 0.8 0.6]);
 xlabel('Deadline','FontSize',fsize)
 ylabel('Temperature / K','FontSize',fsize, 'Rotation', 90)
box on;


%% legend
h=legend('APTM','BWS','O-PBOO', 'Location', 'Best');
pos = get(h,'pos');
% pos(1) = pos(1) - 0.5;
% pos(2) = pos(2) + 0.3;
set(h, 'FontName','Times New Roman','fontsize',fsize ,'box','on');
grid on

saveas(gca, 'deadline_dell9020.eps' ,'epsc2') ;


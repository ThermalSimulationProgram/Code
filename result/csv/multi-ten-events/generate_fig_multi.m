
filename = 'ten_event_deadlinefactor.eps';

kernels = {'cs', 'pboo', 'saptm', 'jour'};
control = [1, 1, 1, 1];

prefix = 'deadline_factor_';

deadlines = 0.8:0.1:2.0;


allTemp = cell(1, 4);

for i = 1:4
    allTemp{i} = zeros(1, numel(deadlines));
end

cel2K = 273.15;

for i = 1 : 4
    if ~control(i)
        continue;
    end
    
    id = 1;
    
    for df = deadlines
        
        name = [ prefix, num2str(df, '%3.1f'), '_', kernels{i}, '_result.csv'];
        
        M = csvread(name);
        
        allTemp{1, i}(id) = M(2, 1)/1000 + cel2K;
        
        id = id + 1;
        
    end  
end


plot(deadlines, allTemp{1}, '-*', deadlines, allTemp{2}, '-d',...
    deadlines, allTemp{3}, '-s', deadlines, allTemp{4}, '-x','MarkerSize',10, 'lineWidth', 1.5);
   

%% post processing

fsize = 16;

set(gca, 'xlim',[min(deadlines)-0.1, max(deadlines)+0.1],'xtick', deadlines,...
    'ylim',[30 50]+cel2K,'FontSize',fsize,...
    'FontName','Times New Roman','Position',[0.13 0.15 0.8 0.7]);
 xlabel('Deadline factor','FontSize',fsize)
 ylabel('Temperature / K','FontSize',fsize, 'Rotation', 90)
box on;


%% legend


h=legend('CS', 'PMPT', 'AAPTM', 'JOUR', 'Location', 'Best');
pos = get(h,'pos');
% pos(1) = pos(1) - 0.5;
% pos(2) = pos(2) + 0.3;
set(h, 'FontName','Times New Roman','fontsize',fsize ,'box','on');
grid on
%title('Peak temperature for different exe factor','fontsize',fsize );

%% saving
%saveas(gca, filename ,'epsc2') ;


        
        

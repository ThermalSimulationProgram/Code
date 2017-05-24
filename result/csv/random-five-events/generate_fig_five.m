
filename = 'ten_event_deadlinefactor.eps';

kernels = {'cs', 'pboo', 'saptm', 'jour'};
control = [1, 1, 1, 1];

prefix = 'S_streams_';

randseq=[ 2 10 4 5 3
    8 9 2 10 3
    6 1 9 4 10
    9 2 5 3 10
    3 7 1 9 8
    4 1 9 8 7
    10 3 5 4 2
    1 6 5 8 3
    2 4 9 1 7
    2 10 9 7 8];

sampleNum = size(randseq, 1);

Tem_all = zeros(sampleNum, 4);

cel2K = 273.15;

for i = 1 : 4
    if ~control(i)
        continue;
    end
   
    
    for j = 1 : size(randseq, 1)
        
        taskids = randseq(j, :);
        name = prefix;
        for id = taskids
            name = [name, 'S', num2str(id), '_'];
            
        end
        
        
        name = [ name, '_', kernels{i}, '_result.csv'];
        
        M = csvread(name);
        
        Tem_all(j, i) = M(2, 1)/1000 + cel2K;
        
    end  
end


tem_f=Tem_all;

txx=1:1:10;

tem_f1=zeros(10,3);
tem_f1(:,1)=tem_f(:,1);
tem_f1(:,2)=tem_f(:,2);
tem_f1(:,3)=tem_f(:,3);
Tklabel='';
for i=1:10
    temp = strcat('''S(',num2str(randseq(i,1)),', ',num2str(randseq(i,2)),', '...
        ,num2str(randseq(i,3)),', ',num2str(randseq(i,4)),', ',num2str(randseq(i,5)),')''');
    if i==1
        Tklabel= strcat(Tklabel,temp);
    else
        Tklabel= strcat(Tklabel,',',temp);
    end
end


fsize = 14;

[AH,H1,H2]=plotyy(txx,tem_f1,txx,tem_f(:,4)','bar','scatter');
set(AH,'Position',[0.14 0.15 0.8 0.6]);
box on;
set(H2,'MarkerFaceColor','red','MarkerEdgeColor','red','SizeData',144,'LineWidth',0.5)

set(AH(1),'xlim',[0.2 10.8],'xtick',[1:1:10],'ylim',[300, 312], 'ytick', 300:3:312, ...
    'FontSize', fsize, 'Xcolor','k','Ycolor','k','FontName','Times New Roman');
set(AH(2),'xlim',[0.2 10.8],'ylim',[300, 324], 'ytick', 300:6:324, 'FontSize', fsize,'Ycolor',...
    'k','FontName','Times New Roman');
set(get(AH(1),'Ylabel'),'string','Peak Temperature','Interpreter',...
    'latex','FontSize', fsize,'FontName','Times New Roman','Rotation', 90);

cmmd = strcat('set(AH(1),''XTickLabel'',{',Tklabel,'})');
eval(cmmd);
set(AH(2),'XTickLabel',[]);

xtl=get(AH(1),'XTickLabel');

xt=get(AH(1),'XTick');

yt=get(AH(1),'YTick');

xtextp=xt;

delta = 0.5;
ytextp=(yt(1)- delta)*ones(1,length(xt));

text(xtextp,ytextp,xtl,'HorizontalAlignment','left','rotation',-30,'fontsize', fsize,'FontName','Times New Roman');

set(AH(1),'XTickLabel',[]);
h=legend('CS','PMPT','AAMPT','JOUR');
        set(h,'FontSize', fsize,'FontName','Times New Roman');
        pos = get(h,'pos');
        pos(1) = pos(1) - 0.06;
        pos(2) = pos(2) + 0.03;
        set(h,'pos',pos);
grid on


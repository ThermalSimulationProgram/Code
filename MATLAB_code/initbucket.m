function [toffs, tons] = newBTS(config)
%
%  INPUT:
%   activeSetIndex: the index of active set, 1*n
%   sleepSetIndex: the index of sleep set, 1*n
%   consistConstSet:Consistency Constant set
%   deadlineConstSet:Deadline Constant set
%   OUTPUT:
%   SleepInterval:Sleep Interval

activeSetIndex  = config.activeSet;
sleepSetIndex   = config.sleepSet;
nactiveStage    = numel(activeSetIndex);
nsleepStage     = numel(sleepSetIndex);
nstages         = nactiveStage + nsleepStage;
consistConstSet = config.ccs;
deadlineConstSet= config.dcs ;
TBET            = config.TBET ;
alldata         = config.offlineData;
rho             = config.rho;
K               = config.K;
slopefuncs      = alldata.slopedata;



rewardfuncs = cell(1, nstages);
numslopes = numel(slopefuncs);
for i = 1 : nstages
    rhoi = K(i);
    rhoId = min(numslopes, max(1, round(rhoi/0.01)));
    rewardfuncs{i} = slopefuncs{rhoId}{i};
end
dynamicdata.rewardfuncs = rewardfuncs;
dynamicdata.wcets = config.wcets;
dynamicdata.rho = rho;
dynamicdata.deltaT = config.deltaT;


tau0 = zeros(1, nstages);
taue = zeros(1, nstages);
tons = taue;

newActiveStagesIndex = activeSetIndex;
nactiveStage1       = size(newActiveStagesIndex,2);
activeSetT          = config.T(activeSetIndex);

resultActiveIndex   = [];
resultSleepIndex    = [];

resultSleepIndex    = [resultSleepIndex, sleepSetIndex];
tau0(sleepSetIndex) = consistConstSet;

lamda_bar = reduceM(newActiveStagesIndex, sleepSetIndex, deadlineConstSet, consistConstSet, nstages);

if ~isempty(lamda_bar)
    phi = min( floor(lamda_bar ./ TBET(newActiveStagesIndex)),  1 : nactiveStage1);
else
    phi = [];
end
while(nactiveStage1 > 0)
   
    if isempty(phi)
        sss=1;
    end
    [minphi, indexAtminphi] = min(phi);
    
    nstage2minphi = indexAtminphi(end);
    

    stageIndex2minphi = (nactiveStage1 - nstage2minphi + 1) : nactiveStage1; 
    
    tempsetIndex = newActiveStagesIndex( stageIndex2minphi );
    
 
    if minphi > 0
        T0 = activeSetT( stageIndex2minphi );
        
        [~, index] = sort(T0,'descend'); 
        
        
        
        if( nstage2minphi > minphi )
            resultActiveIndex = [resultActiveIndex, tempsetIndex(index(minphi+1:end))];
        end
       
        resultSleepIndex = [resultSleepIndex, tempsetIndex(index(1:minphi))];
        
        tau0(tempsetIndex(index(1:minphi))) = TBET(tempsetIndex(index(1:minphi)));
    else
       
        resultActiveIndex = [resultActiveIndex, tempsetIndex];
    end
   
    phi = phi(nstage2minphi + 1 : end) - minphi;
    nactiveStage1 = nactiveStage1 - nstage2minphi;
    newActiveStagesIndex = newActiveStagesIndex(1 : nactiveStage1);
    phi = min( phi,1 : nactiveStage1 );
end


ASS_N = numel(resultActiveIndex);
SSS_N = numel(resultSleepIndex);



[resultSleepIndex]    = sort(resultSleepIndex);
[resultActiveIndex]   = sort(resultActiveIndex);


if(SSS_N)
    ASS_N1 = ASS_N;
    SSS_N1 = SSS_N;
    SSS1 = resultSleepIndex;
    ASS1 = resultActiveIndex;
    lamda_bar_e = Reduce_Matrxi(SSS1, SSS_N1, ASS1, ASS_N1, ...
        zeros(1,ASS_N1), ASS_N1, deadlineConstSet, nstages);
    lamda_bar_e = lamda_bar_e -  (tril(ones( SSS_N1, SSS_N1)) * tau0(SSS1)')';
    
    
    while( SSS_N1 > 0 )
        
        minlamda    = min(lamda_bar_e);
        lamda_index = find(lamda_bar_e==minlamda);
        lamda_index = lamda_index(end);
        
        tempsetIndex     = SSS1((SSS_N1-lamda_index+1):SSS_N1);
        if minlamda <= 0
 
        tinvs = zeros(size(tempsetIndex));
        tvlds = 100*ones(size(tempsetIndex));
        else

dynamicdata.breakToffs = alldata.breakToffs(:,tempsetIndex);
dynamicdata.slopes = alldata.slopes(:,tempsetIndex);
dynamicdata.numValidData = alldata.numValidData(tempsetIndex);

[tinvs, tvlds] = aPtm(minlamda, tempsetIndex, tau0(tempsetIndex), dynamicdata);
        end
        taue(tempsetIndex) = tinvs;
        tons(tempsetIndex) = tvlds;
        %taue(tempsetIndex) = minlamda/lamda_index;
        
        lamda_bar_e   = lamda_bar_e( lamda_index+1 : SSS_N1)- minlamda;
        
        SSS_N1      = SSS_N1-lamda_index;
        SSS1        = SSS1(1:SSS_N1);
    end
    toffs = tau0 + taue;

    toffs = floor((toffs - config.tswons)/config.deltaT) * config.deltaT;
    toffs = max(toffs, 0);
else
    toffs = zeros(1, nstages);
    tons  = 100*ones(1, nstages);
end
tons = ceil((tons + config.tswons)/config.deltaT) * config.deltaT;



    


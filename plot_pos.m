#! /usr/bin/octave -qf

isCommandLine=0;
if(isCommandLine == 1)
	arg_list = argv();
	matFilename = arg_list{1}
else
	matFilename = 'circle.mat'
end


load(matFilename);

data = csvread(dataFile);
truth = csvread(groundTruth);

dataTime = data(:,1)-min(data(:,1));

truth(:,2) = truth(:,2) - mean(truth(:,2));
truth(:,3) = truth(:,3) - mean(truth(:,3));
truth(:,4) = truth(:,4) - mean(truth(:,4));
globalMax = max(max(truth(:,2:4)));

if(shouldNormalize==1)
	truth(:,2) = truth(:,2)./(globalMax);
	truth(:,4) = truth(:,4)./(globalMax);
	truth(:,3) = truth(:,3)./(globalMax);
%	truth(:,2) = truth(:,2)./(max(abs(truth(:,2))));
%	truth(:,4) = truth(:,4)./(max(abs(truth(:,4))));
	truthMax = globalMax;
else
	truth(:,2) = truth(:,2)./(2*max(truth(:,2)));
	truth(:,3) = truth(:,3)./(2*max(truth(:,3)));
	truth(:,4) = truth(:,4)./(2*max(truth(:,4)));
end

data(:,2) = data(:,2) - mean(data(:,2));
data(:,3) = data(:,3) - mean(data(:,3));
data(:,4) = data(:,4) - mean(data(:,4));

globalMax = max(max(data(:,2:4)));

if(shouldNormalize==1)
	data(:,2) = data(:,2)./(max(data(:,2)));
	data(:,3) = data(:,3)./(max(data(:,3)));
	dataMax = globalMax;
else
	data(:,2) = data(:,2)./(2*max(data(:,2)));
	data(:,3) = data(:,3)./(2*max(data(:,3)));
	data(:,4) = data(:,4)./(2*max(data(:,4)));
end


globalMax = max([truthMax,globalMax]);
h = figure();


maxX = max([max(data(:,2)),max(truth(:,2))]);
maxY = max([max(data(:,3)),max(truth(:,4))]);
maxZ = max([max(data(:,4)),max(truth(:,3))]);

plot3(data(:,2),data(:,3),data(:,4),'LineWidth',5);
if (shouldNormalize==0)
	xlim([-maxX,maxX]);
	ylim([-maxY,maxY]);
	zlim([-maxZ,maxZ]);
else
	xlim([-2,2]);
	ylim([-2,2]);
	zlim([-.05,.05]);
end
hold on
plot3(truth(:,2),truth(:,4),truth(:,3),'r','LineWidth',5);
if (shouldNormalize==0)
	xlim([-maxX,maxX]);
	ylim([-maxY,maxY]);
	zlim([-maxZ,maxZ]);
else
	xlim([-2,2]);
	ylim([-2,2]);
	zlim([-.05,.05]);

end
hold off

imageFilename = strrep(matFilename,".mat",".png");
print(imageFilename);

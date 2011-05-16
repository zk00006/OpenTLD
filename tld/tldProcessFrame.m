% Copyright 2011 Zdenek Kalal
%
% This file is part of TLD.
% 
% TLD is free software: you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation, either version 3 of the License, or
% (at your option) any later version.
% 
% TLD is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU General Public License for more details.
% 
% You should have received a copy of the GNU General Public License
% along with TLD.  If not, see <http://www.gnu.org/licenses/>.

function tld = tldProcessFrame(tld,i)

I = tld.source.idx(i); % get current index
tld.img{I} = img_get(tld.source,I); % grab frame from camera / load image

% TRACKER  ----------------------------------------------------------------

[tBB tConf tValid tld] = tldTracking(tld,tld.bb(:,I-1),I-1,I); % frame-to-frame tracking (MedianFlow)

% DETECTOR ----------------------------------------------------------------

[dBB dConf tld] = tldDetection(tld,I); % detect appearances by cascaded detector (variance filter -> ensemble classifier -> nearest neightbour)

% INTEGRATOR --------------------------------------------------------------

DT = 1; if isempty(dBB), DT = 0; end % is detector defined?
TR = 1; if isempty(tBB), TR = 0; end % is tracker defined?

if TR % if tracker is defined
    
    % copy tracker's result
    tld.bb(:,I)  = tBB;
    tld.conf(I)  = tConf;
    tld.size(I)  = 1;
    tld.valid(I) = tValid;
    if tld.PRINT_DEBUG==1
        fprintf('Status [%d]: Tracking is occuring\n',I);
    end
    if DT % if detections are also defined
        if tld.PRINT_DEBUG==1
            fprintf('Status [%d]: Detection is occuring\n',I);
        end
        [cBB,cConf,cSize] = bb_cluster_confidence(dBB,dConf); % cluster detections
        id = bb_overlap(tld.bb(:,I),cBB) < 0.5 & cConf > tld.conf(I); % get indexes of all clusters that are far from tracker and are more confident then the tracker
        
        if sum(id) == 1 % if there is ONE such a cluster, re-initialize the tracker
            if tld.PRINT_DEBUG==1
                fprintf('Status [%d]: Re-Init BB\n',I);
            end
            tld.bb(:,I)  = cBB(:,id);
            tld.conf(I)  = cConf(:,id);
            tld.size(I)  = cSize(:,id);
            tld.valid(I) = 0; 
            
        else % othervide adjust the tracker's trajectory
            if tld.PRINT_DEBUG==1
                fprintf('Status [%d]: Average BB\n',I);
            end
            idTr = bb_overlap(tBB,tld.dt{I}.bb) > 0.7;  % get indexes of close detections
            tld.bb(:,I) = mean([repmat(tBB,1,10) tld.dt{I}.bb(:,idTr)],2);  % weighted average trackers trajectory with the close detections
            
        end
    end
    
else % if tracker is not defined
    if DT % and detector is defined
        if tld.PRINT_DEBUG==1
            fprintf('Status [%d]: Detection is occuring\n',I);
        end
        [cBB,cConf,cSize] = bb_cluster_confidence(dBB,dConf); % cluster detections
        
        if length(cConf) == 1 % and if there is just a single cluster, re-initalize the tracker
            if tld.PRINT_DEBUG==1
                fprintf('Status [%d]: Re-Init BB\n',I);
            end
            tld.bb(:,I)  = cBB;
            tld.conf(I)  = cConf;
            tld.size(I)  = cSize;
            tld.valid(I) = 0; 
        end
    end
end

% LEARNING ----------------------------------------------------------------

if tld.control.update_detector && tld.valid(I) == 1
    tld = tldLearning(tld,I);
end

% display drawing: get center of bounding box and save it to a drawn line
if ~isnan(tld.bb(1,I))
        if tld.PRINT_DEBUG==1
            fprintf('Status [%d]:Learning is Occuring\n',I);
        end
    tld.draw(:,end+1) = bb_center(tld.bb(:,I));
    if tld.plot.draw == 0, tld.draw(:,end) = nan; end
else
    tld.draw = zeros(2,0);
end

if tld.control.drop_img && I > 2, tld.img{I-1} = {}; end % forget previous image









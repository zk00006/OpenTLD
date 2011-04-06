% Overlap of neighbouring bbox as a function of shift

shift = 0.01:0.01:0.5;

for i = 1:length(shift)
    over1(i) = (1-shift(i)) / (2 - (1-shift(i)));
    over2(i) = (1-shift(i))^2 / (2 - (1-shift(i))^2);
end
clf;
plot(shift,over1,'b'); hold on;
plot(shift,over2,'r')
legend('shift in one direction','shift in two direction');
grid on;

print(gcf,'-depsc2','bbox_overlap_on_shift.eps');



x = [1; 2; 4; 6; 8;];

v3_belgium_openmp = [0.014; 0.021; 0.011; 0.008; 0.056];
v3_youtube_openmp = [4.13; 2.31; 1.75; 1.51; 1.36];
v3_dble_openmp = [0.15; 0.20; 0.26; 0.36; 0.4;];
v3_cielskian_openmp = [11.05; 9.2; 6.6; 5.3; 4.4;];
v3_NACA_openmp = [0.21; 0.30; 0.34; 0.43; 0.50];


v3_belgium_cilk = [0.014; 0.0185; 0.021; 0.024; 0.024];
v3_youtube_cilk = [4.13; 2.01; 1.02; 0.82; 0.83];
v3_dble_cilk = [0.15; 0.23; 0.25; 0.28; 0.31];
v3_cielskian_cilk = [11.05; 5.8; 2.7; 2.1; 1.86];
v3_NACA_cilk = [0.21; 0.41; 0.35; 0.38; 0.55];


v4_belgium_openmp = [0.28; 0.45; 0.30; 0.27; 0.23];
v4_youtube_openmp = [11.19; 9.60; 7.7; 7.5; 7.09];
v4_dble_openmp = [0.38; 0.33; 0.22; 0.20; 0.17];
v4_cielskian_openmp = [2.72; 1.8; 1.18; 1.04; 0.88];
v4_NACA_openmp = [1.428; 1.16; 0.67; 0.56; 0.53];


v4_belgium_cilk = [0.28; 0.36; 0.26; 0.21; 0.20];
v4_youtube_cilk = [11.19; 3.80; 2.10; 1.46; 1.19];
v4_dble_cilk = [0.38; 0.26; 0.19; 0.15; 0.13];
v4_cielskian_cilk = [2.72; 0.96; 0.57; 0.40; 0.33];
v4_NACA_cilk = [1.428; 0.90; 0.60; 0.44; 0.39];

v4_belgium_pthreads = [0.28; 0.32; 0.28; 0.28; 0.25];
v4_dble_pthreads = [0.38; 0.40; 0.33; 0.35; 0.36];
v4_cielskian_pthreads = [2.72; 5.30; 5.34; 5.29; 5.41];
v4_NACA_pthreads = [1.428; 0.96; 0.61; 0.69; 0.64];


% Belgium for v3 and v4
y1 = v3_belgium_openmp
y2 = v3_belgium_cilk
figure
plot(x,y1,x,y2)
title('Execution time of belgium relative to cores for each parallel implementation of V3')
xlabel('Number of Cores')
ylabel('Execution time in seconds')
legend({'v3 belgium openmp','v3 belgium cilk' },'Location','northeast')
set(gcf, 'Position',  [100, 100, 700, 500])
print(gcf,'belgium_v3.png','-dpng','-r300');   

y1 = v4_belgium_openmp
y2 = v4_belgium_cilk
y3 = v4_belgium_pthreads
figure
plot(x,y1,x,y2,x,y3)
title('Execution time of belgium relative to cores for each parallel implementation of V4')
xlabel('Number of Cores')
ylabel('Execution time in seconds')
legend({'v4 belgium openmp','v4 belgium cilk', 'v4 belgium pthreads' },'Location','northeast')
set(gcf, 'Position',  [100, 100, 700, 500])
print(gcf,'belgium_v4.png','-dpng','-r300');


% Youtube for v3 and v4
y1 = v3_youtube_openmp
y2 = v3_youtube_cilk
figure
plot(x,y1,x,y2)
title('Execution time of youtube relative to cores for each parallel implementation of V3')
xlabel('Number of Cores')
ylabel('Execution time in seconds')
legend({'v3 youtube openmp','v3 youtube cilk' },'Location','northeast')
set(gcf, 'Position',  [100, 100, 700, 500])
print(gcf,'youtube_v3.png','-dpng','-r300');

y1 = v4_youtube_openmp
y2 = v4_youtube_cilk
h=figure
plot(x,y1,x,y2)
title('Execution time of youtube relative to cores for each parallel implementation of V4')
xlabel('Number of Cores')
ylabel('Execution time in seconds')
legend({'v4 youtube openmp','v4 youtube cilk' },'Location','northeast')
set(gcf, 'Position',  [100, 100, 700, 500])
print(gcf,'youtube_v4.png','-dpng','-r300');


% dble for v3 and v4
y1 = v3_dble_openmp
y2 = v3_dble_cilk
figure
plot(x,y1,x,y2)
title('Execution time of dble relative to cores for each parallel implementation of V3')
xlabel('Number of Cores')
ylabel('Execution time in seconds')
legend({'v3 dble openmp','v3 dble cilk' },'Location','northeast')
set(gcf, 'Position',  [100, 100, 700, 500])
print(gcf,'dble_v3.png','-dpng','-r300');

y1 = v4_dble_openmp
y2 = v4_dble_cilk
y3 = v4_dble_pthreads
figure
plot(x,y1,x,y2,x,y3)
title('Execution time of dble relative to cores for each parallel implementation of V4')
xlabel('Number of Cores')
ylabel('Execution time in seconds')
legend({'v4 dble openmp','v4 dble cilk', 'v4 dble pthreads' },'Location','northeast')
set(gcf, 'Position',  [100, 100, 700, 500])
print(gcf,'dble_v4.png','-dpng','-r300');


% cielskian for v3 and v4
y1 = v3_cielskian_openmp
y2 = v3_cielskian_cilk
figure
plot(x,y1,x,y2)
title('Execution time of cielskian relative to cores for each parallel implementation of V3')
xlabel('Number of Cores')
ylabel('Execution time in seconds')
legend({'v3 cielskian openmp','v3 cielskian cilk' },'Location','northeast')
set(gcf, 'Position',  [100, 100, 700, 500])
print(gcf,'cielskian_v3.png','-dpng','-r300');

y1 = v4_cielskian_openmp
y2 = v4_cielskian_cilk
y3 = v4_cielskian_pthreads
figure
plot(x,y1,x,y2,x,y3)
title('Execution time of cielskian relative to cores for each parallel implementation of V4')
xlabel('Number of Cores')
ylabel('Execution time in seconds')
legend({'v4 cielskian openmp','v4 cielskian cilk', 'v4 cielskian pthreads' },'Location','northeast')
set(gcf, 'Position',  [100, 100, 700, 500])
print(gcf,'cielskian_v4.png','-dpng','-r300');


% NACA for v3 and v4
y1 = v3_NACA_openmp
y2 = v3_NACA_cilk
figure
plot(x,y1,x,y2)
title('Execution time of NACA relative to cores for each parallel implementation of V3')
xlabel('Number of Cores')
ylabel('Execution time in seconds')
legend({'v3 NACA openmp','v3 NACA cilk' },'Location','northeast')
set(gcf, 'Position',  [100, 100, 700, 500])
print(gcf,'NACA_v3.jpg','-dpng','-r300');

y1 = v4_NACA_openmp
y2 = v4_NACA_cilk
y3 = v4_NACA_pthreads
figure
plot(x,y1,x,y2,x,y3)
title('Execution time of NACA relative to cores for each parallel implementation of V4')
xlabel('Number of Cores')
ylabel('Execution time in seconds')
legend({'v4 NACA openmp','v4 NACA cilk', 'v4 NACA pthreads' },'Location','northeast')
set(gcf, 'Position',  [100, 100, 700, 500])
print(gcf,'NACA_v4.jpg','-dpng','-r300');

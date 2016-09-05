#include "dev.h"

int main(int argc, char **argv){

    if(argc != 5){
        fprintf(stderr,"\nusage FeatureSelection <training set> <evaluating set> <testing set> <search space configuration file>\n");
        exit(-1);
    }
    
    SearchSpace *s = NULL;
    int i;
    double time;
    timer tic, toc;
    FILE *f = NULL;
    TransferFunc optTransfer = NULL;
    Subgraph *Train = NULL, *Evaluate = NULL, *Merge = NULL, *Test = NULL, *newTrain = NULL, *newTest = NULL;
 
    Train = ReadSubgraph(argv[1]);
    Evaluate = ReadSubgraph(argv[2]);
    Test = ReadSubgraph(argv[3]);
    s = ReadSearchSpaceFromFile(argv[4], _PSO_);
    optTransfer = S2TransferFunction;
    
    for (i = 0; i < Train->nfeats; i++){
	s->LB[i] = 0;
	s->UB[i] = 1;
    }
    
    fprintf(stderr,"\nInitializing search space ... ");
    InitializeSearchSpace(s, _PSO_);
    fprintf(stderr,"\nOk\n");
    
    fflush(stderr); fprintf(stderr,"\nRunning PSO ... ");
    gettimeofday(&tic,NULL);
    runPSO(s, FeatureSelection, Train, Evaluate, optTransfer);
    gettimeofday(&toc,NULL);
    fflush(stderr); fprintf(stderr,"\nOK\n");
    
    Merge = opf_MergeSubgraph(Train, Evaluate);
        
    fflush(stderr); fprintf(stderr,"\nWriting new training and testing sets ...\n");
    newTrain = CreateSubgraphFromSelectedFeatures(Merge, s->g);
    newTest = CreateSubgraphFromSelectedFeatures(Test, s->g);
    fprintf(stderr, "\nTraining set\n");
    WriteSubgraph(newTrain, "training.pso.dat");
    fprintf(stderr, "\n\nTesting set\n");
    WriteSubgraph(newTest, "testing.pso.dat");
    fflush(stderr); fprintf(stderr,"\nOK\n");
    
    f = fopen("best_feats.txt", "a");
    fprintf(f, "%d", (int)s->g[0]);
    for(i = 1; i < Train->nfeats; i++){
	    fprintf(f, " %d", (int)s->g[i]);
    }
    fprintf(f, "\n");
    fclose(f);
    
    fflush(stderr); fprintf(stderr,"\nDeallocating memory ...");
    DestroySubgraph(&Train);
    DestroySubgraph(&Evaluate);
    DestroySubgraph(&Merge);
    DestroySubgraph(&Test);
    DestroySubgraph(&newTrain);
    DestroySubgraph(&newTest);
    fflush(stderr); fprintf(stderr,"\nOK\n");
    
    time = ((toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001)/1000.0;
    fprintf(stdout, "\nRunning time: %f seconds\n", time); fflush(stderr);
    
    f = fopen("optimization.time","a");
    fprintf(f,"%f\n",time);
    fclose(f);
    
    DestroySearchSpace(&s, _PSO_);
    
    return 0;
}
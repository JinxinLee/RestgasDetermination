void testTGeoHMatrices(){
	cout << "Checking where rotation and translation in a TGeoHMatrix are.\n";
	TGeoHMatrix rotation;
	TGeoHMatrix translation;
	double trans[3] = {1,2,3};
	rotation.RotateX(25.0);
	translation.SetTranslation(trans);
	TGeoHMatrix combiTrans = translation * rotation;

	cout << "matrix print function:\n";
	combiTrans.Print();
	cout << "last row was skipped.\n\n";

	cout << "homogenous matrix:\n";
	double *combi = new double[16];
	combiTrans.GetHomogenousMatrix(combi);
	for(int i=0; i<16; i++){
		if(i%4==0) cout << "\n";
		cout << combi[i] << "\t";
	}
	cout << "\n\nwe can see the rotaion is unchanged, but the translation\n";
	cout << "is now on the fourth row.\n";
}

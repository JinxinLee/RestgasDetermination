char* testconfigstuff()
{
    char* default_name = (char*)"cbmsim";
    char* workdir = getenv("VMCWORKDIR");
    if(NULL == workdir)
    {
      cout<<" MARKE 1"<<endl;
        return default_name;
    }

    // Open file with output tree name
    FILE* file = fopen(Form("%s/config/rootmanager.dat",workdir), "r");
    // If file does not exist -> default
    if(NULL == file)
    {
      cout<<" MARKE 2"<<endl;
        return default_name;
    }
    // If file is empty -> default
    char str[100];
    if(NULL == fgets(str, 100, file))
    {
      cout<<" MARKE 3"<<endl;
        fclose(file);
        return default_name;
    }
    // If file does not contain treename key -> default
    char* treename = new char[100];
    if(1 != sscanf(str, "treename=%s", treename))
    {
      cout<<" MARKE 4"<<endl;
        fclose(file);
        delete[] treename;
        return default_name;
    }
    // Close file and return read value
    fclose(file);
      cout<<" MARKE 5"<<endl;
    return treename;
}

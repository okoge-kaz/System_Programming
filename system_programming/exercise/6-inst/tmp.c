char *path = "cg.dot";
    if (isFileExist(path) == 1) {
        // }\n を消す
        FILE *ifp = fopen("cg.dot", "r");
        FILE *ofp = fopen("cg.dot", "w");
        char buf[1024];
        while (fgets(buf, sizeof(buf), ifp) != NULL) {
            if (strcmp(buf, "}") == 0) {
                // }\n を消す
                continue;
            }else if(fputs(buf, ofp) == EOF){
                LOG("fputs error\n");
                exit(1);
            }
        }
        fclose(ifp);
        fclose(ofp);
        // 書き加える
        FILE *fp = fopen("cg.dot", "a");
        if (fp == NULL) {
            LOG("Failed to open cg.dot\n");
            return;
        }
        fprintf(fp, "%s -> %s\n", addr2name(call_site), addr2name(addr));
        fprintf(fp, "}\n");
        fclose(fp);
    } else {
        FILE *f = fopen("cg.dot", "w");
        if (f != NULL) {
            fprintf(f, "strict digraph G {\n");
            fprintf(f, "%s -> %s\n", addr2name(call_site), addr2name(addr));
            fprintf(f, "}\n");
            fclose(f);
        }
    }
void IterateTasksList(TTask* task, TString& text)
{
  text+=Form("\"%s\" \t with name \"%s\" and Title \"%s\"\n",task->ClassName(),task->GetName(),task->GetTitle());
  if (task->GetListOfTasks()==NULL) return;
  for(TObject* task2: *(task->GetListOfTasks()) )
  {
    IterateTasksList((TTask*)task2,text);
  }
  return;
}

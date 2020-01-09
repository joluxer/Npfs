
AUX_SOURCE_DIRECTORY(Messages npMessages)
AUX_SOURCE_DIRECTORY(Fcalls   npFcalls)
AUX_SOURCE_DIRECTORY(Resources npResources)
AUX_SOURCE_DIRECTORY(. npServer)

ADD_LIBRARY(Npfs STATIC
  ${npMessages}
  ${npFcalls}
  ${npResources}
  ${npServer}
)

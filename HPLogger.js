
function doGet(e) {
  Logger.log(e);

  try
  {
    
    retval=LogHPRun(e.parameter.Status);
    
  }
  catch(e)
  {
    
    retval=ContentService.createTextOutput("Error");

  }
  return retval;
}


function LogHPRun(Status)
{
  var StatsSheet = SpreadsheetApp.getActive().getSheetByName("HPStats");
  var now=new Date();
  var statArray=Status.split(',');
  var outarray=[now];
  outarray=outarray.concat(statArray);

  StatsSheet.appendRow(outarray);
  return ContentService
    .createTextOutput("OK")
    .setMimeType(ContentService.MimeType.TEXT);

 
}
/*
 *  ======== package.xs ========
 *
 */

/*
 *  ======== getLibs ========
 */
function getLibs(prog)
{
  var lib = null;
   
  if(prog.build.target.isa == '674' || prog.build.target.isa == '64P' ){            
       if ( this.C6ACCEL.watermark == false ) {
              lib = "lib/C6Accel.l" + prog.build.target.isa;
        }
       else {
           lib = null;
        }

      print("    will link with " + this.$name + ":" + lib);
       
  }
                
    return (lib);
}

/*
 *  ======== getSects ========
 */
function getSects()
{
    var template = null;

    if (Program.build.target.isa == "64P") {
        template = "ti/c6accel/link.xdt";
    }
    
    if (Program.build.target.isa == "674") {
        template = "ti/c6accel/link.xdt";
    }

    return (template);
}

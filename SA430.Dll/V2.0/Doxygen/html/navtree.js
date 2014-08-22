var NAVTREE =
[
  [ "SA430 Interface Dynamic Link Library", "index.html", [
    [ "Class List", "annotated.html", [
      [ "_sa430UsbDevice", "class__sa430_usb_device.html", null ],
      [ "_sFrameDecoderState", "struct__s_frame_decoder_state.html", null ],
      [ "_sRegEntry", "class__s_reg_entry.html", null ],
      [ "cDeviceDriver", "classc_device_driver.html", null ],
      [ "cDriver", "classc_driver.html", null ],
      [ "cEvent", "classc_event.html", null ],
      [ "cMutex", "classc_mutex.html", null ],
      [ "cRegAccess", "classc_reg_access.html", null ],
      [ "cThread", "classc_thread.html", null ],
      [ "cUsbDetect", "classc_usb_detect.html", null ],
      [ "sa430Frame", "structsa430_frame.html", null ],
      [ "sa430Status", "structsa430_status.html", null ],
      [ "sa430UsbDevice", "structsa430_usb_device.html", null ],
      [ "sComPortDescr", "structs_com_port_descr.html", null ],
      [ "sError", "structs_error.html", null ],
      [ "sFrame", "structs_frame.html", null ],
      [ "sPortSetting", "structs_port_setting.html", null ],
      [ "cRegAccess::sRegEntry", "structc_reg_access_1_1s_reg_entry.html", null ],
      [ "sUsbDeviceInfo", "structs_usb_device_info.html", null ],
      [ "TThread< T >", "class_t_thread.html", null ]
    ] ],
    [ "Class Index", "classes.html", null ],
    [ "Class Members", "functions.html", null ],
    [ "File List", "files.html", [
      [ "sa430/cDeviceDriver.cpp", "c_device_driver_8cpp.html", null ],
      [ "sa430/cDeviceDriver.h", "c_device_driver_8h.html", null ],
      [ "sa430/cDriver.cpp", "c_driver_8cpp.html", null ],
      [ "sa430/cDriver.h", "c_driver_8h.html", null ],
      [ "sa430/cEvent.cpp", "c_event_8cpp.html", null ],
      [ "sa430/cEvent.h", "c_event_8h.html", null ],
      [ "sa430/cMutex.cpp", "c_mutex_8cpp.html", null ],
      [ "sa430/cMutex.h", "c_mutex_8h.html", null ],
      [ "sa430/cRegAccess.cpp", "c_reg_access_8cpp.html", null ],
      [ "sa430/cRegAccess.h", "c_reg_access_8h.html", null ],
      [ "sa430/cThread.cpp", "c_thread_8cpp.html", null ],
      [ "sa430/cThread.h", "c_thread_8h.html", null ],
      [ "sa430/cUsbDetect.cpp", "c_usb_detect_8cpp.html", null ],
      [ "sa430/cUsbDetect.h", "c_usb_detect_8h.html", null ],
      [ "sa430/dllmain.cpp", "dllmain_8cpp.html", null ],
      [ "sa430/sa430.cpp", "sa430_8cpp.html", null ],
      [ "sa430/sa430.h", "sa430_8h.html", null ],
      [ "sa430/sa430_global.h", "sa430__global_8h.html", null ],
      [ "sa430/sa430Cmd.h", "sa430_cmd_8h.html", null ],
      [ "sa430/sa430TypeDef.h", "sa430_type_def_8h.html", null ]
    ] ],
    [ "File Members", "globals.html", null ]
  ] ]
];

function createIndent(o,domNode,node,level)
{
  if (node.parentNode && node.parentNode.parentNode)
  {
    createIndent(o,domNode,node.parentNode,level+1);
  }
  var imgNode = document.createElement("img");
  if (level==0 && node.childrenData)
  {
    node.plus_img = imgNode;
    node.expandToggle = document.createElement("a");
    node.expandToggle.href = "javascript:void(0)";
    node.expandToggle.onclick = function() 
    {
      if (node.expanded) 
      {
        $(node.getChildrenUL()).slideUp("fast");
        if (node.isLast)
        {
          node.plus_img.src = node.relpath+"ftv2plastnode.png";
        }
        else
        {
          node.plus_img.src = node.relpath+"ftv2pnode.png";
        }
        node.expanded = false;
      } 
      else 
      {
        expandNode(o, node, false);
      }
    }
    node.expandToggle.appendChild(imgNode);
    domNode.appendChild(node.expandToggle);
  }
  else
  {
    domNode.appendChild(imgNode);
  }
  if (level==0)
  {
    if (node.isLast)
    {
      if (node.childrenData)
      {
        imgNode.src = node.relpath+"ftv2plastnode.png";
      }
      else
      {
        imgNode.src = node.relpath+"ftv2lastnode.png";
        domNode.appendChild(imgNode);
      }
    }
    else
    {
      if (node.childrenData)
      {
        imgNode.src = node.relpath+"ftv2pnode.png";
      }
      else
      {
        imgNode.src = node.relpath+"ftv2node.png";
        domNode.appendChild(imgNode);
      }
    }
  }
  else
  {
    if (node.isLast)
    {
      imgNode.src = node.relpath+"ftv2blank.png";
    }
    else
    {
      imgNode.src = node.relpath+"ftv2vertline.png";
    }
  }
  imgNode.border = "0";
}

function newNode(o, po, text, link, childrenData, lastNode)
{
  var node = new Object();
  node.children = Array();
  node.childrenData = childrenData;
  node.depth = po.depth + 1;
  node.relpath = po.relpath;
  node.isLast = lastNode;

  node.li = document.createElement("li");
  po.getChildrenUL().appendChild(node.li);
  node.parentNode = po;

  node.itemDiv = document.createElement("div");
  node.itemDiv.className = "item";

  node.labelSpan = document.createElement("span");
  node.labelSpan.className = "label";

  createIndent(o,node.itemDiv,node,0);
  node.itemDiv.appendChild(node.labelSpan);
  node.li.appendChild(node.itemDiv);

  var a = document.createElement("a");
  node.labelSpan.appendChild(a);
  node.label = document.createTextNode(text);
  a.appendChild(node.label);
  if (link) 
  {
    a.href = node.relpath+link;
  } 
  else 
  {
    if (childrenData != null) 
    {
      a.className = "nolink";
      a.href = "javascript:void(0)";
      a.onclick = node.expandToggle.onclick;
      node.expanded = false;
    }
  }

  node.childrenUL = null;
  node.getChildrenUL = function() 
  {
    if (!node.childrenUL) 
    {
      node.childrenUL = document.createElement("ul");
      node.childrenUL.className = "children_ul";
      node.childrenUL.style.display = "none";
      node.li.appendChild(node.childrenUL);
    }
    return node.childrenUL;
  };

  return node;
}

function showRoot()
{
  var headerHeight = $("#top").height();
  var footerHeight = $("#nav-path").height();
  var windowHeight = $(window).height() - headerHeight - footerHeight;
  navtree.scrollTo('#selected',0,{offset:-windowHeight/2});
}

function expandNode(o, node, imm)
{
  if (node.childrenData && !node.expanded) 
  {
    if (!node.childrenVisited) 
    {
      getNode(o, node);
    }
    if (imm)
    {
      $(node.getChildrenUL()).show();
    } 
    else 
    {
      $(node.getChildrenUL()).slideDown("fast",showRoot);
    }
    if (node.isLast)
    {
      node.plus_img.src = node.relpath+"ftv2mlastnode.png";
    }
    else
    {
      node.plus_img.src = node.relpath+"ftv2mnode.png";
    }
    node.expanded = true;
  }
}

function getNode(o, po)
{
  po.childrenVisited = true;
  var l = po.childrenData.length-1;
  for (var i in po.childrenData) 
  {
    var nodeData = po.childrenData[i];
    po.children[i] = newNode(o, po, nodeData[0], nodeData[1], nodeData[2],
        i==l);
  }
}

function findNavTreePage(url, data)
{
  var nodes = data;
  var result = null;
  for (var i in nodes) 
  {
    var d = nodes[i];
    if (d[1] == url) 
    {
      return new Array(i);
    }
    else if (d[2] != null) // array of children
    {
      result = findNavTreePage(url, d[2]);
      if (result != null) 
      {
        return (new Array(i).concat(result));
      }
    }
  }
  return null;
}

function initNavTree(toroot,relpath)
{
  var o = new Object();
  o.toroot = toroot;
  o.node = new Object();
  o.node.li = document.getElementById("nav-tree-contents");
  o.node.childrenData = NAVTREE;
  o.node.children = new Array();
  o.node.childrenUL = document.createElement("ul");
  o.node.getChildrenUL = function() { return o.node.childrenUL; };
  o.node.li.appendChild(o.node.childrenUL);
  o.node.depth = 0;
  o.node.relpath = relpath;

  getNode(o, o.node);

  o.breadcrumbs = findNavTreePage(toroot, NAVTREE);
  if (o.breadcrumbs == null)
  {
    o.breadcrumbs = findNavTreePage("index.html",NAVTREE);
  }
  if (o.breadcrumbs != null && o.breadcrumbs.length>0)
  {
    var p = o.node;
    for (var i in o.breadcrumbs) 
    {
      var j = o.breadcrumbs[i];
      p = p.children[j];
      expandNode(o,p,true);
    }
    p.itemDiv.className = p.itemDiv.className + " selected";
    p.itemDiv.id = "selected";
    $(window).load(showRoot);
  }
}


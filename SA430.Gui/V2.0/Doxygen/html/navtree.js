var NAVTREE =
[
  [ "SA430 Spectrum Analyzer Graphic User Interface", "index.html", [
    [ "Class List", "annotated.html", [
      [ "_sCalMemory", "struct__s_cal_memory.html", null ],
      [ "_sDeviceInfo", "struct__s_device_info.html", null ],
      [ "_sFrqParameterBuffer", "struct__s_frq_parameter_buffer.html", null ],
      [ "_sFrqRange", "struct__s_frq_range.html", null ],
      [ "_sFrqRBW", "struct__s_frq_r_b_w.html", null ],
      [ "_sFrqRefLevel", "struct__s_frq_ref_level.html", null ],
      [ "_sFrqRefLevGain", "struct__s_frq_ref_lev_gain.html", null ],
      [ "_sFrqSetting", "struct__s_frq_setting.html", null ],
      [ "_sFrqValues", "struct__s_frq_values.html", null ],
      [ "_sMarkerInfo", "struct__s_marker_info.html", null ],
      [ "_sProgHeader", "struct__s_prog_header.html", null ],
      [ "_sSettingGrid", "struct__s_setting_grid.html", null ],
      [ "_sSettingGui", "struct__s_setting_gui.html", null ],
      [ "_sSettingMarker", "struct__s_setting_marker.html", null ],
      [ "_sSettingTrace", "struct__s_setting_trace.html", null ],
      [ "_sSpectrum", "struct__s_spectrum.html", null ],
      [ "_sSpectrumOffset", "struct__s_spectrum_offset.html", null ],
      [ "appCurve", "classapp_curve.html", null ],
      [ "appFwUpdater", "classapp_fw_updater.html", null ],
      [ "appGrid", "classapp_grid.html", null ],
      [ "appMarker", "classapp_marker.html", null ],
      [ "appPlot", "classapp_plot.html", null ],
      [ "appReportCsv", "classapp_report_csv.html", null ],
      [ "appSettings", "classapp_settings.html", null ],
      [ "appStatusBar", "classapp_status_bar.html", null ],
      [ "cEvent", "classc_event.html", null ],
      [ "DlgFrqSettings", "class_dlg_frq_settings.html", null ],
      [ "drvSA430", "classdrv_s_a430.html", null ],
      [ "MainWindow", "class_main_window.html", null ],
      [ "sCalCoeffs", "structs_cal_coeffs.html", null ],
      [ "sCalFrqRange", "structs_cal_frq_range.html", null ],
      [ "sCalibrationData", "structs_calibration_data.html", null ],
      [ "sMarkerCfg", "structs_marker_cfg.html", null ],
      [ "sRefLevGain", "structs_ref_lev_gain.html", null ],
      [ "sStatus", "structs_status.html", null ],
      [ "sStatusSA430", "structs_status_s_a430.html", null ],
      [ "sStatusSpectrum", "structs_status_spectrum.html", null ],
      [ "sTraceCfg", "structs_trace_cfg.html", null ]
    ] ],
    [ "Class Index", "classes.html", null ],
    [ "File List", "files.html", [
      [ "appCurve.h", null, null ],
      [ "appFwUpdater.h", null, null ],
      [ "appgrid.h", null, null ],
      [ "appMarker.h", null, null ],
      [ "appPlot.h", null, null ],
      [ "appReportCsv.h", null, null ],
      [ "appSettings.h", null, null ],
      [ "appStatusbar.h", null, null ],
      [ "appTypedef.h", null, null ],
      [ "cEvent.h", null, null ],
      [ "dlgFrqSettings.h", null, null ],
      [ "drvSA430.h", null, null ],
      [ "mainwindow.h", null, null ]
    ] ]
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


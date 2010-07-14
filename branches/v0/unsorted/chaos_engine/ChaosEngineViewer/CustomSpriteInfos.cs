using System;
using System.Collections.Generic;
using System.Text;
using System.Xml.Serialization;

namespace ChaosEngineViewer {
  [XmlRoot ("infos")]
  public class CustomSpriteInfos {
    [XmlElement ("info", typeof (CustomSpriteInfo))]
    public List<CustomSpriteInfo> _infos = new List<CustomSpriteInfo> ();

    //[XmlElement ("infos")]
    //public List<CustomSpriteInfo> Infos {
    //  get {
    //    return (_infos);
    //  }
    //  set {
    //    _infos = value;
    //  }
    //}
  }
}

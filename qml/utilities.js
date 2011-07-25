function pad(number, length) {
  var ret = '' + number;

  while (ret.length < length) {
    ret = '0' + ret;
  }

  return ret;
}

function pretty_duration(duration) {
  var ret = "";

  var hours = Math.floor(duration / (60*60));
  var minutes = Math.floor(duration / 60) % 60;
  var seconds = duration % 60;

  if (hours) {
    return hours + ":" + pad(minutes, 2) + " hours";
  } else if (minutes) {
    return minutes + " minutes";
  } else {
    return seconds + " seconds";
  }
}

function pretty_size(bytes) {
  if (bytes <= 1000) {
    return bytes + " bytes";
  }
  if (bytes <= 1000*1000) {
    return Math.round(bytes / 1000) + " KB";
  }
  if (bytes <= 1000*1000*1000) {
    return Math.round(bytes / (1000*1000)) + " MB";
  }
  return (bytes / (1000*1000*1000)).toFixed(1) + " GB";
}

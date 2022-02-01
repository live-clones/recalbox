//
// Created by bkg2k on 08/10/2020.
//

#include "Raspberry.h"
#include <alsa/asoundlib.h>
#include <utils/Log.h>

void Raspberry::SetRoute(Raspberry::Output output)
{
  int err = 0;
  static snd_ctl_t *handle = nullptr;
  snd_ctl_elem_info_t* info = nullptr;
  snd_ctl_elem_id_t* id = nullptr;
  snd_ctl_elem_value_t* control = nullptr;
  snd_ctl_elem_info_alloca(&info);
  snd_ctl_elem_id_alloca(&id);
  snd_ctl_elem_value_alloca(&control);

  // Convert output to string
  std::string value = Strings::ToString((int)output);

  // Open
  if (snd_ctl_ascii_elem_id_parse(id, "numid=3") != 0) { LOG(LogError) << "[Alsa] Cannot get ID 3"; return; }
  if ((err = snd_ctl_open(&handle, "hw:0", 0)) < 0) { LOG(LogError) << "[Alsa] Cannot open Card 0, Error: " << snd_strerror(err); return; }

  // Get informations
  snd_ctl_elem_info_set_id(info, id);
  if ((err = snd_ctl_elem_info(handle, info)) < 0) { LOG(LogError) << "[Alsa] Cannot find the given element from control hw:0, Error: " << snd_strerror(err); snd_ctl_close(handle); return; }
  snd_ctl_elem_info_get_id(info, id); // TODO: Really usefull?
  // Create control
  snd_ctl_elem_value_set_id(control, id);
  // Read current value
  if ((err = snd_ctl_elem_read(handle, control)) < 0) { LOG(LogError) << "[Alsa] Cannot read the given element from control hw:0, Error: " << snd_strerror(err); snd_ctl_close(handle); return; }
  // Parse
  if ((err = snd_ctl_ascii_value_parse(handle, control, info, value.data())) < 0) { LOG(LogError) << "[Alsa] Cannot parse the given element from control hw:0, Error: " << snd_strerror(err); snd_ctl_close(handle); return; }
  // Write
  if ((err = snd_ctl_elem_write(handle, control)) < 0) { LOG(LogError) << "[Alsa] Cannot write the given element from control hw:0, Error: " << snd_strerror(err); snd_ctl_close(handle); return; }
  // Close
  snd_ctl_close(handle);
}

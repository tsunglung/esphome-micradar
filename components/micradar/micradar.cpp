#include "micradar.h"

#include "esphome/core/log.h"

#include <utility>

namespace esphome {
namespace micradar {

static const char *const TAG = "micradar";

// Prints the component's configuration data. dump_config() prints all of the component's configuration
// items in an easy-to-read format, including the configuration key-value pairs.
void MICRADARComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "MicRadar:");
#ifdef USE_TEXT_SENSOR
  LOG_TEXT_SENSOR(" ", "Equipment State Text Sensor", this->equipment_state_text_sensor_);
  LOG_TEXT_SENSOR(" ", "Product Model Text Sensor", this->product_model_text_sensor_);
  LOG_TEXT_SENSOR(" ", "Product ID Text Sensor", this->product_id_text_sensor_);
  LOG_TEXT_SENSOR(" ", "Hardware Model Text Sensor", this->hardware_model_text_sensor_);
  LOG_TEXT_SENSOR(" ", "Firware Verison Text Sensor", this->firware_version_text_sensor_);
  LOG_TEXT_SENSOR(" ", "Keep Away Text Sensor", this->keep_away_text_sensor_);
  LOG_TEXT_SENSOR(" ", "Motion Status Text Sensor", this->motion_status_text_sensor_);
  LOG_TEXT_SENSOR(" ", "Custom Mode End Text Sensor", this->custom_mode_end_text_sensor_);
#endif
#ifdef USE_BINARY_SENSOR
  LOG_BINARY_SENSOR(" ", "Has Target Binary Sensor", this->has_target_binary_sensor_);
#endif
#ifdef USE_SENSOR
  LOG_SENSOR(" ", "Custom Presence Of Detection Sensor", this->custom_presence_of_detection_sensor_);
  LOG_SENSOR(" ", "Movement Signs Sensor", this->movement_signs_sensor_);
  LOG_SENSOR(" ", "Custom Motion Distance Sensor", this->custom_motion_distance_sensor_);
  LOG_SENSOR(" ", "Custom Spatial Static Sensor", this->custom_spatial_static_value_sensor_);
  LOG_SENSOR(" ", "Custom Spatial Motion Sensor", this->custom_spatial_motion_value_sensor_);
  LOG_SENSOR(" ", "Custom Motion Speed Sensor", this->custom_motion_speed_sensor_);
  LOG_SENSOR(" ", "Custom Mode Num Sensor", this->custom_mode_num_sensor_);
#endif
#ifdef USE_SWITCH
  LOG_SWITCH(" ", "Underly Open Function Switch", this->underlying_open_function_switch_);
#endif
#ifdef USE_BUTTON
  LOG_BUTTON(" ", "Restart Button", this->restart_button_);
  LOG_BUTTON(" ", "Custom Set End Button", this->custom_set_end_button_);
#endif
#ifdef USE_SELECT
  LOG_SELECT(" ", "Scene Mode Select", this->scene_mode_select_);
  LOG_SELECT(" ", "Unman Time Select", this->unman_time_select_);
  LOG_SELECT(" ", "Existence Boundary Select", this->existence_boundary_select_);
  LOG_SELECT(" ", "Motion Boundary Select", this->motion_boundary_select_);
#endif
#ifdef USE_NUMBER
  LOG_NUMBER(" ", "Sensitivity Number", this->sensitivity_number_);
  LOG_NUMBER(" ", "Custom Mode Number", this->custom_mode_number_);
  LOG_NUMBER(" ", "Existence Threshold Number", this->existence_threshold_number_);
  LOG_NUMBER(" ", "Motion Threshold Number", this->motion_threshold_number_);
  LOG_NUMBER(" ", "Motion Trigger Time Number", this->motion_trigger_number_);
  LOG_NUMBER(" ", "Motion To Rest Time Number", this->motion_to_rest_number_);
  LOG_NUMBER(" ", "Custom Unman Time Number", this->custom_unman_time_number_);
#endif
}

// Initialisation functions
void MICRADARComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up MicRadar...");
  this->check_uart_settings(115200);
#ifdef USE_NUMBER
  if (this->custom_mode_number_ != nullptr) {
    this->custom_mode_number_->publish_state(0);  // Zero out the custom mode
  }
#endif
  if (this->custom_mode_num_sensor_ != nullptr) {
    this->custom_mode_num_sensor_->publish_state(0);
  }
  if (this->custom_mode_end_text_sensor_ != nullptr) {
    this->custom_mode_end_text_sensor_->publish_state("Not in custom mode");
  }
  this->set_custom_end_mode();
  this->poll_time_base_func_check_ = true;
  this->check_dev_inf_sign_ = true;
  this->sg_start_query_data_ = STANDARD_FUNCTION_QUERY_PRODUCT_MODE;
  this->sg_data_len_ = 0;
  this->sg_frame_len_ = 0;
  this->sg_recv_data_state_ = FRAME_IDLE;
  this->s_output_info_switch_flag_ = OUTPUT_SWITCH_INIT;

  memset(this->c_product_mode_, 0, PRODUCT_BUF_MAX_SIZE);
  memset(this->c_product_id_, 0, PRODUCT_BUF_MAX_SIZE);
  memset(this->c_firmware_version_, 0, PRODUCT_BUF_MAX_SIZE);
  memset(this->c_hardware_model_, 0, PRODUCT_BUF_MAX_SIZE);
  memset(this->sg_frame_prase_buf_, 0, FRAME_BUF_MAX_SIZE);
  memset(this->sg_frame_buf_, 0, FRAME_BUF_MAX_SIZE);

  this->set_interval(8000, [this]() { this->update_(); });
  ESP_LOGCONFIG(TAG, "Set up MicRadar complete");
}

// Timed polling of radar data
void MICRADARComponent::update_() {
  this->get_radar_output_information_switch();  // Query the key status every so often
  this->poll_time_base_func_check_ = true;      // Query the base functionality information at regular intervals
}

// main loop
void MICRADARComponent::loop() {
  uint8_t byte;

  // Is there data on the serial port
  while (this->available()) {
    this->read_byte(&byte);
    this->micradar_split_data_frame_(byte);  // split data frame
  }

  if ((this->s_output_info_switch_flag_ == OUTPUT_SWTICH_OFF) &&
      (this->sg_start_query_data_ > CUSTOM_FUNCTION_QUERY_TIME_OF_ENTER_UNMANNED) && (!this->check_dev_inf_sign_)) {
    this->sg_start_query_data_ = STANDARD_FUNCTION_QUERY_SCENE_MODE;
  } else if ((this->s_output_info_switch_flag_ == OUTPUT_SWITCH_ON) &&
             (this->sg_start_query_data_ < CUSTOM_FUNCTION_QUERY_EXISTENCE_BOUNDARY) && (!this->check_dev_inf_sign_)) {
    this->sg_start_query_data_ = CUSTOM_FUNCTION_QUERY_EXISTENCE_BOUNDARY;
  } else if (this->check_dev_inf_sign_ && (this->sg_start_query_data_ > STANDARD_FUNCTION_QUERY_HARDWARE_MODE)) {
    // First time power up information polling
    this->sg_start_query_data_ = STANDARD_FUNCTION_QUERY_PRODUCT_MODE;
  }

  // Polling Functions
  if (this->poll_time_base_func_check_) {
    switch (this->sg_start_query_data_) {
      case STANDARD_FUNCTION_QUERY_PRODUCT_MODE:
        this->get_product_mode();
        this->sg_start_query_data_++;
        break;
      case STANDARD_FUNCTION_QUERY_PRODUCT_ID:
        this->get_product_id();
        this->sg_start_query_data_++;
        break;
      case STANDARD_FUNCTION_QUERY_FIRMWARE_VERSION:
        this->get_product_mode();
        this->get_product_id();
        this->get_firmware_version();
        this->sg_start_query_data_++;
        break;
      case STANDARD_FUNCTION_QUERY_HARDWARE_MODE:  // Above is the equipment information
        this->get_product_mode();
        this->get_product_id();
        this->get_hardware_model();
        this->sg_start_query_data_++;
        this->check_dev_inf_sign_ = false;
        break;
      case STANDARD_FUNCTION_QUERY_SCENE_MODE:
        if (is_function_supported(STANDARD_FUNCTION_QUERY_SCENE_MODE))
          this->get_scene_mode();
        this->sg_start_query_data_++;
        break;
      case STANDARD_FUNCTION_QUERY_SENSITIVITY:
        if (is_function_supported(STANDARD_FUNCTION_QUERY_SENSITIVITY))
          this->get_sensitivity();
        this->sg_start_query_data_++;
        break;
      case STANDARD_FUNCTION_QUERY_UNMANNED_TIME:
        if (is_function_supported(STANDARD_FUNCTION_QUERY_UNMANNED_TIME))
          this->get_unmanned_time();
        this->sg_start_query_data_++;
        break;
      case STANDARD_FUNCTION_QUERY_HUMAN_STATUS:
        if (is_function_supported(STANDARD_FUNCTION_QUERY_HUMAN_STATUS))
          this->get_human_status();
        this->sg_start_query_data_++;
        break;
      case STANDARD_FUNCTION_QUERY_HUMAN_MOTION_INF:
        if (is_function_supported(STANDARD_FUNCTION_QUERY_HUMAN_MOTION_INF))
          this->get_human_motion_info();
        this->sg_start_query_data_++;
        break;
      case STANDARD_FUNCTION_QUERY_BODY_MOVE_PARAMETER:
        if (is_function_supported(STANDARD_FUNCTION_QUERY_BODY_MOVE_PARAMETER))
          this->get_body_motion_params();
        this->sg_start_query_data_++;
        break;
      case STANDARD_FUNCTION_QUERY_KEEPAWAY_STATUS:  // The above is the basic functional information
        if (is_function_supported(STANDARD_FUNCTION_QUERY_KEEPAWAY_STATUS))
          this->get_keep_away();
        this->sg_start_query_data_++;
        break;
      case STANDARD_FUNCTION_QUERY_CUSTOM_MODE:
        if (is_function_supported(STANDARD_FUNCTION_QUERY_CUSTOM_MODE))
          this->get_custom_mode();
        this->sg_start_query_data_++;
        break;
      case STANDARD_FUNCTION_QUERY_HEARTBEAT_STATE:
        if (is_function_supported(STANDARD_FUNCTION_QUERY_HEARTBEAT_STATE))
          this->get_heartbeat_packet();
        this->sg_start_query_data_++;
        break;
      case STANDARD_FUNCTION_QUERY_BREATHE_STATE:
        if (is_function_supported(STANDARD_FUNCTION_QUERY_BREATHE_STATE)) {
          this->get_breathe_packet();
        }
        this->sg_start_query_data_++;
        break;
      case STANDARD_FUNCTION_QUERY_SLEEP_STATE:
        if (is_function_supported(STANDARD_FUNCTION_QUERY_SLEEP_STATE)) {
          this->get_sleep_packet();
        }
        this->sg_start_query_data_++;
        break;
      case CUSTOM_FUNCTION_QUERY_EXISTENCE_BOUNDARY:
        this->get_existence_boundary();
        this->sg_start_query_data_++;
        break;
      case CUSTOM_FUNCTION_QUERY_MOTION_BOUNDARY:
        this->get_motion_boundary();
        this->sg_start_query_data_++;
        break;
      case CUSTOM_FUNCTION_QUERY_EXISTENCE_THRESHOLD:
        this->get_existence_threshold();
        this->sg_start_query_data_++;
        break;
      case CUSTOM_FUNCTION_QUERY_MOTION_THRESHOLD:
        this->get_motion_threshold();
        this->sg_start_query_data_++;
        break;
      case CUSTOM_FUNCTION_QUERY_MOTION_TRIGGER_TIME:
        this->get_motion_trigger_time();
        this->sg_start_query_data_++;
        break;
      case CUSTOM_FUNCTION_QUERY_MOTION_TO_REST_TIME:
        this->get_motion_to_rest_time();
        this->sg_start_query_data_++;
        break;
      case CUSTOM_FUNCTION_QUERY_TIME_OF_ENTER_UNMANNED:
        this->get_custom_unman_time();
        this->sg_start_query_data_++;
        if (this->s_output_info_switch_flag_ == OUTPUT_SWTICH_OFF) {
          this->poll_time_base_func_check_ = false;  // Avoiding high-speed polling that can cause the device to jam
        }
        break;
      case UNDERLY_FUNCTION_QUERY_HUMAN_STATUS:
        this->get_human_status();
        this->sg_start_query_data_++;
        break;
      case UNDERLY_FUNCTION_QUERY_SPATIAL_STATIC_VALUE:
        this->get_spatial_static_value();
        this->sg_start_query_data_++;
        break;
      case UNDERLY_FUNCTION_QUERY_SPATIAL_MOTION_VALUE:
        this->get_spatial_motion_value();
        this->sg_start_query_data_++;
        break;
      case UNDERLY_FUNCTION_QUERY_DISTANCE_OF_STATIC_OBJECT:
        this->get_distance_of_static_object();
        this->sg_start_query_data_++;
        break;
      case UNDERLY_FUNCTION_QUERY_DISTANCE_OF_MOVING_OBJECT:
        this->get_distance_of_moving_object();
        this->sg_start_query_data_++;
        break;
      case UNDERLY_FUNCTION_QUERY_TARGET_MOVEMENT_SPEED:
        this->get_target_movement_speed();
        this->sg_start_query_data_++;
        this->poll_time_base_func_check_ = false;  // Avoiding high-speed polling that can cause the device to jam
        break;
      default:
        break;
    }
  }
}

// Calculate CRC check digit, total length, tail byte will be removed here.
static uint8_t get_frame_crc_sum(const uint8_t *data, int len) {
  unsigned int crc_sum = 0;
  for (int i = 0; i < len - 3; i++) {
    crc_sum += data[i];
  }
  return crc_sum & 0xff;
}

// Check that the check digit is correct
static int get_frame_check_status(uint8_t *data, int len) {
  uint8_t crc_sum = get_frame_crc_sum(data, len);
  uint8_t verified = data[len - 3];
  return (verified == crc_sum) ? 1 : 0;
}

static inline int16_t hex_to_signed_int(const uint8_t *buffer, uint8_t offset) {
  uint16_t hex_val = (buffer[offset] << 8) | buffer[offset + 1];
  int16_t dec_val = static_cast<int16_t>(hex_val);
  if (dec_val & 0x8000) {
    dec_val -= 32768;
    dec_val = -dec_val;
  }
  return dec_val;
}

// split data frame
void MICRADARComponent::micradar_split_data_frame_(uint8_t value) {
  switch (this->sg_recv_data_state_) {
    case FRAME_IDLE:  // starting value
      if (FRAME_HEADER1_VALUE == value) {
        this->sg_recv_data_state_ = FRAME_HEADER2;
      }
      break;
    case FRAME_HEADER2:
      if (FRAME_HEADER2_VALUE == value) {
        this->sg_frame_buf_[0] = FRAME_HEADER1_VALUE;
        this->sg_frame_buf_[1] = FRAME_HEADER2_VALUE;
        this->sg_recv_data_state_ = FRAME_CTL_WORD;
      } else {
        this->sg_recv_data_state_ = FRAME_IDLE;
        ESP_LOGD(TAG, "FRAME_IDLE ERROR value:%x", value);
      }
      break;
    case FRAME_CTL_WORD:
      this->sg_frame_buf_[2] = value;
      this->sg_recv_data_state_ = FRAME_CMD_WORD;
      break;
    case FRAME_CMD_WORD:
      this->sg_frame_buf_[3] = value;
      this->sg_recv_data_state_ = FRAME_DATA_LEN_H;
      break;
    case FRAME_DATA_LEN_H:
      if (value <= 4) {
        this->sg_data_len_ = value * 256;
        this->sg_frame_buf_[4] = value;
        this->sg_recv_data_state_ = FRAME_DATA_LEN_L;
      } else {
        this->sg_data_len_ = 0;
        this->sg_recv_data_state_ = FRAME_IDLE;
        ESP_LOGD(TAG, "FRAME_DATA_LEN_H ERROR value:%x", value);
      }
      break;
    case FRAME_DATA_LEN_L:
      this->sg_data_len_ += value;
      if (this->sg_data_len_ > 32) {
        ESP_LOGD(TAG, "len=%d, FRAME_DATA_LEN_L ERROR value:%x", this->sg_data_len_, value);
        this->sg_data_len_ = 0;
        this->sg_recv_data_state_ = FRAME_IDLE;
      } else {
        this->sg_frame_buf_[5] = value;
        this->sg_frame_len_ = 6;
        this->sg_recv_data_state_ = FRAME_DATA_BYTES;
      }
      break;
    case FRAME_DATA_BYTES:
      this->sg_data_len_ -= 1;
      this->sg_frame_buf_[this->sg_frame_len_++] = value;
      if (this->sg_data_len_ <= 0) {
        this->sg_recv_data_state_ = FRAME_DATA_CRC;
      }
      break;
    case FRAME_DATA_CRC:
      this->sg_frame_buf_[this->sg_frame_len_++] = value;
      this->sg_recv_data_state_ = FRAME_TAIL1;
      break;
    case FRAME_TAIL1:
      if (FRAME_TAIL1_VALUE == value) {
        this->sg_recv_data_state_ = FRAME_TAIL2;
      } else {
        this->sg_recv_data_state_ = FRAME_IDLE;
        this->sg_frame_len_ = 0;
        this->sg_data_len_ = 0;
        ESP_LOGD(TAG, "FRAME_TAIL1 ERROR value:%x", value);
      }
      break;
    case FRAME_TAIL2:
      if (FRAME_TAIL2_VALUE == value) {
        this->sg_frame_buf_[this->sg_frame_len_++] = FRAME_TAIL1_VALUE;
        this->sg_frame_buf_[this->sg_frame_len_++] = FRAME_TAIL2_VALUE;
        memcpy(this->sg_frame_prase_buf_, this->sg_frame_buf_, this->sg_frame_len_);
        if (get_frame_check_status(this->sg_frame_prase_buf_, this->sg_frame_len_)) {
          this->micradar_parse_data_frame_(this->sg_frame_prase_buf_, this->sg_frame_len_);
        } else {
          ESP_LOGD(TAG, "frame check failer!");
        }
      } else {
        ESP_LOGD(TAG, "FRAME_TAIL2 ERROR value:%x", value);
      }
      memset(this->sg_frame_prase_buf_, 0, FRAME_BUF_MAX_SIZE);
      memset(this->sg_frame_buf_, 0, FRAME_BUF_MAX_SIZE);
      this->sg_frame_len_ = 0;
      this->sg_data_len_ = 0;
      this->sg_recv_data_state_ = FRAME_IDLE;
      break;
    default:
      this->sg_recv_data_state_ = FRAME_IDLE;
  }
}

// Parses data frames related to product information
void MICRADARComponent::micradar_frame_parse_product_information_(uint8_t command, uint8_t *data) {
  uint16_t length = encode_uint16(data[FRAME_LENGTH_INDEX], data[FRAME_LENGTH_INDEX + 1]);
  switch (command) {
    case COMMAND_PRODUCT_MODE:
      if (length > PRODUCT_BUF_MAX_SIZE) {
        ESP_LOGD(TAG, "Reply: get product_mode error!");
        break;
      } else if (this->product_model_text_sensor_ != nullptr) {
        memset(this->c_product_mode_, 0, PRODUCT_BUF_MAX_SIZE);
        memcpy(this->c_product_mode_, &data[FRAME_DATA_INDEX], length);
        this->product_model_text_sensor_->publish_state(this->c_product_mode_);
      }
      break;
    case COMMAND_PRODUCT_ID:
      if (length > PRODUCT_BUF_MAX_SIZE) {
        ESP_LOGD(TAG, "Reply: get productId error!");
        break;
      } else if (this->product_id_text_sensor_ != nullptr) {
        memset(this->c_product_id_, 0, PRODUCT_BUF_MAX_SIZE);
        memcpy(this->c_product_id_, &data[FRAME_DATA_INDEX], length);
        this->product_id_text_sensor_->publish_state(this->c_product_id_);
      }
      break;
    case COMMAND_HARDWARE_MODEL:
      if (length > PRODUCT_BUF_MAX_SIZE) {
        ESP_LOGD(TAG, "Reply: get hardware_model error!");
        break;
      } else if (this->hardware_model_text_sensor_ != nullptr) {
        memset(this->c_hardware_model_, 0, PRODUCT_BUF_MAX_SIZE);
        memcpy(this->c_hardware_model_, &data[FRAME_DATA_INDEX], length);
        this->hardware_model_text_sensor_->publish_state(this->c_hardware_model_);
      }
      break;
    case COMMAND_FIRMWARE_VERSION:
      if (length > PRODUCT_BUF_MAX_SIZE) {
        ESP_LOGD(TAG, "Reply: get firmwareVersion error!");
        break;
      } else if (this->firware_version_text_sensor_ != nullptr) {
        memset(this->c_firmware_version_, 0, PRODUCT_BUF_MAX_SIZE);
        memcpy(this->c_firmware_version_, &data[FRAME_DATA_INDEX], length);
        this->firware_version_text_sensor_->publish_state(this->c_firmware_version_);
      }
      break;
  }
}

// Parsing the underlying open parameters
void MICRADARComponent::micradar_frame_parse_open_underlying_information_(uint8_t command, uint8_t *data) {
  uint16_t length = encode_uint16(data[FRAME_LENGTH_INDEX], data[FRAME_LENGTH_INDEX + 1]);
  switch (command) {
    case 0x00:
#ifdef USE_SWITCH
      if (this->underlying_open_function_switch_ != nullptr) {
        this->underlying_open_function_switch_->publish_state(
            data[FRAME_DATA_INDEX]);  // Underlying Open Parameter Switch Status Updates
      }
#endif
      if (data[FRAME_DATA_INDEX]) {
        this->s_output_info_switch_flag_ = OUTPUT_SWITCH_ON;
      } else {
        this->s_output_info_switch_flag_ = OUTPUT_SWTICH_OFF;
      }
      break;

    case 0x01:
      if (this->custom_spatial_static_value_sensor_ != nullptr) {
        this->custom_spatial_static_value_sensor_->publish_state(data[FRAME_DATA_INDEX]);
      }
      if (this->custom_presence_of_detection_sensor_ != nullptr) {
        this->custom_presence_of_detection_sensor_->publish_state(data[FRAME_DATA_INDEX + 1] * 0.5f);
      }
      if (this->custom_spatial_motion_value_sensor_ != nullptr) {
        this->custom_spatial_motion_value_sensor_->publish_state(data[FRAME_DATA_INDEX + 2]);
      }
      if (this->custom_motion_distance_sensor_ != nullptr) {
        this->custom_motion_distance_sensor_->publish_state(data[FRAME_DATA_INDEX + 3] * 0.5f);
      }
      if (this->custom_motion_speed_sensor_ != nullptr) {
        this->custom_motion_speed_sensor_->publish_state((data[FRAME_DATA_INDEX + 4] - 10) * 0.5f);
      }
      break;

    case 0x06:
    case 0x86:
      // none:0x00  close_to:0x01  far_away:0x02
      if ((this->keep_away_text_sensor_ != nullptr) && (data[FRAME_DATA_INDEX] < 3)) {
        this->keep_away_text_sensor_->publish_state(S_KEEP_AWAY_STR[data[FRAME_DATA_INDEX]]);
      }
      break;

    case 0x07:
    case 0x87:
      if ((this->movement_signs_sensor_ != nullptr)) {
        this->movement_signs_sensor_->publish_state(data[FRAME_DATA_INDEX]);
      }
      break;

#ifdef USE_NUM
    case 0x08:
    case 0x88:
      if ((this->existence_threshold_number_ != nullptr)) {
        this->existence_threshold_number_->publish_state(data[FRAME_DATA_INDEX]);
      }
      break;

    case 0x09:
    case 0x89:
      if ((this->motion_threshold_number_ != nullptr)) {
        this->motion_threshold_number_->publish_state(data[FRAME_DATA_INDEX]);
      }
      break;

    case 0x0C:
    case 0x8C:
      if ((this->motion_trigger_number_ != nullptr)) {
        uint32_t motion_trigger_time = encode_uint32(data[FRAME_DATA_INDEX], data[FRAME_DATA_INDEX + 1],
                                                    data[FRAME_DATA_INDEX + 2], data[FRAME_DATA_INDEX + 3]);
        this->motion_trigger_number_->publish_state(motion_trigger_time);
      }
      break;

    case 0x0D:
    case 0x8D:
      if ((this->motion_to_rest_number_ != nullptr)) {
        uint32_t move_to_rest_time = encode_uint32(data[FRAME_DATA_INDEX], data[FRAME_DATA_INDEX + 1],
                                                  data[FRAME_DATA_INDEX + 2], data[FRAME_DATA_INDEX + 3]);
        this->motion_to_rest_number_->publish_state(move_to_rest_time);
      }
      break;

    case 0x0E:
    case 0x8E:
      if ((this->custom_unman_time_number_ != nullptr)) {
        uint32_t enter_unmanned_time = encode_uint32(data[FRAME_DATA_INDEX], data[FRAME_DATA_INDEX + 1],
                                                    data[FRAME_DATA_INDEX + 2], data[FRAME_DATA_INDEX + 3]);
        float custom_unmanned_time = enter_unmanned_time / 1000.0;
        this->custom_unman_time_number_->publish_state(custom_unmanned_time);
      }
      break;
#endif

#ifdef USE_SELECT
    case 0x0A:
    case 0x8A:
      if ((this->existence_boundary_select_ != nullptr) &&
             (this->existence_boundary_select_->has_index(data[FRAME_DATA_INDEX] - 1))) {
        this->existence_boundary_select_->publish_state(S_BOUNDARY_STR[data[FRAME_DATA_INDEX] - 1]);
      }
      break;

    case 0x0B:
    case 0x8B:
      if ((this->motion_boundary_select_ != nullptr) &&
      (this->motion_boundary_select_->has_index(data[FRAME_DATA_INDEX] - 1))) {
        this->motion_boundary_select_->publish_state(S_BOUNDARY_STR[data[FRAME_DATA_INDEX] - 1]);
      }
      break;

    case 0x81:
      if (this->custom_spatial_static_value_sensor_ != nullptr) {
        this->custom_spatial_static_value_sensor_->publish_state(data[FRAME_DATA_INDEX]);
      }
      break;

    case 0x82:
      if (this->custom_spatial_motion_value_sensor_ != nullptr) {
        this->custom_spatial_motion_value_sensor_->publish_state(data[FRAME_DATA_INDEX]);
      }
      break;

    case 0x83:
      if (this->custom_presence_of_detection_sensor_ != nullptr) {
        this->custom_presence_of_detection_sensor_->publish_state(
          S_PRESENCE_OF_DETECTION_RANGE_STR[data[FRAME_DATA_INDEX]]);
      }
      break;

    case 0x84:
      if (this->custom_motion_distance_sensor_ != nullptr) {
        this->custom_motion_distance_sensor_->publish_state(data[FRAME_DATA_INDEX] * 0.5f);
      }
      break;

    case 0x85:
      if (this->custom_motion_speed_sensor_ != nullptr) {
        this->custom_motion_speed_sensor_->publish_state((data[FRAME_DATA_INDEX] - 10) * 0.5f);
      }
    break;

#endif

    default:
      ESP_LOGD(TAG, "[%s] No found COMMAND_WORD(%02X) in Frame", __FUNCTION__, data[FRAME_COMMAND_WORD_INDEX]);
      break;
  }
}

void MICRADARComponent::micradar_parse_data_frame_(uint8_t *data, uint8_t len) {
  uint8_t control = data[FRAME_CONTROL_WORD_INDEX];
  uint8_t command = data[FRAME_COMMAND_WORD_INDEX];
  switch (control) {
    case CONTROL_MAIN: {
      if ((this->equipment_state_text_sensor_ != nullptr) && (command == 0x01)) {
        this->equipment_state_text_sensor_->publish_state("Equipment Normal");
      } else if (command == 0x02) {
        ESP_LOGD(TAG, "Reply: query restart packet");
      } else if (this->equipment_state_text_sensor_ != nullptr) {
        this->equipment_state_text_sensor_->publish_state("Equipment Abnormal");
      }
    } break;
    case CONTROL_PRODUCT_INFORMATION: {
      this->micradar_frame_parse_product_information_(command, data);
    } break;
    case CONTROL_WORK: {
      this->micradar_frame_parse_work_status_(command, data);
    } break;
    case CONTROL_RADAR_RANGE: {
      this->micradar_frame_parse_radar_range_(command, data);
    } break;
    case CONTROL_UNDERLYING_FUNCTION: {
      this->micradar_frame_parse_open_underlying_information_(command, data);
    } break;
    case CONTROL_HUMAN_INFORMATION: {
      this->micradar_frame_parse_human_information_(command, data);
    } break;
    case CONTROL_HUMAN_BREATHE_INFORMATION: {
      this->micradar_frame_parse_human_breathe_information_(command, data);
    } break;
    case CONTROL_HUMAN_TRACKING_INFORMATION: {
      this->micradar_frame_parse_human_tracking_information_(command, data);
    } break;
    case CONTROL_HUMAN_SLEEP_INFORMATION: {
      this->micradar_frame_parse_human_sleep_information_(command, data);
    } break;
    case CONTROL_HUMAN_HEART_RATE_TYPE_INFORMATION: {
      this->micradar_frame_parse_human_heart_rate_information_(command, data);
    } break;
    case CONTROL_HUMAN_COUNT_TYPE_INFORMATION: {
      this->micradar_frame_parse_human_count_information_(command, data);
    } break;
    default:
      ESP_LOGD(TAG, "control word:0x%02X not found", control);
      break;
  }
}

void MICRADARComponent::micradar_frame_parse_work_status_(uint8_t command, uint8_t *data) {
  uint16_t length = encode_uint16(data[FRAME_LENGTH_INDEX], data[FRAME_LENGTH_INDEX + 1]);
  switch (command) {
    case 0x1:
      ESP_LOGD(TAG, "Reply: get radar init status 0x%02X", data[FRAME_DATA_INDEX]);
      break;
    case 0x07:
  #ifdef USE_SELECT
      if ((this->scene_mode_select_ != nullptr) && (this->scene_mode_select_->has_index(data[FRAME_DATA_INDEX]))) {
        this->scene_mode_select_->publish_state(S_SCENE_STR[data[FRAME_DATA_INDEX]]);
      }
  #endif
      break;
    case 0x08:
    case 0x88:
  #ifdef USE_NUMBER
      } else if ((this->sensitivity_number_ != nullptr) &&
               ((data[FRAME_COMMAND_WORD_INDEX] == 0x08) || (data[FRAME_COMMAND_WORD_INDEX] == 0x88))) {
      // 1-3
      this->sensitivity_number_->publish_state(data[FRAME_DATA_INDEX]);
  #endif
      break;
    case 0x09:
      // 1-4
      if (this->custom_mode_num_sensor_ != nullptr) {
        this->custom_mode_num_sensor_->publish_state(data[FRAME_DATA_INDEX]);
      }
#ifdef USE_NUMBER
      if (this->custom_mode_number_ != nullptr) {
        this->custom_mode_number_->publish_state(0);
      }
#endif
      if (this->custom_mode_end_text_sensor_ != nullptr) {
        this->custom_mode_end_text_sensor_->publish_state("Setup in progress...");
      }
      break;
    case 0x81:
      ESP_LOGD(TAG, "Reply: get radar init status 0x%02X", data[FRAME_DATA_INDEX]);
      break;
    case 0x87:
#ifdef USE_SELECT
      if ((this->scene_mode_select_ != nullptr) && (this->scene_mode_select_->has_index(data[FRAME_DATA_INDEX]))) {
        this->scene_mode_select_->publish_state(S_SCENE_STR[data[FRAME_DATA_INDEX]]);
      } else {
        ESP_LOGD(TAG, "Select has index offset %d Error", data[FRAME_DATA_INDEX]);
      }
#endif
      break;
    case 0x89:
      if (data[FRAME_DATA_INDEX] == 0) {
        if (this->custom_mode_end_text_sensor_ != nullptr) {
          this->custom_mode_end_text_sensor_->publish_state("Not in custom mode");
        }
#ifdef USE_NUMBER
        if (this->custom_mode_number_ != nullptr) {
          this->custom_mode_number_->publish_state(0);
        }
#endif
        if (this->custom_mode_num_sensor_ != nullptr) {
          this->custom_mode_num_sensor_->publish_state(data[FRAME_DATA_INDEX]);
        }
      } else {
        if (this->custom_mode_num_sensor_ != nullptr) {
          this->custom_mode_num_sensor_->publish_state(data[FRAME_DATA_INDEX]);
        }
      }
      break;
    default:
      ESP_LOGD(TAG, "[%s] No found COMMAND_WORD(%02X) in Frame", __FUNCTION__, data[FRAME_COMMAND_WORD_INDEX]);
      break;
  }
}

void MICRADARComponent::micradar_frame_parse_radar_range_(uint8_t command, uint8_t *data) {
  uint16_t length = encode_uint16(data[FRAME_LENGTH_INDEX], data[FRAME_LENGTH_INDEX + 1]);
  switch (command) {
    case COMMAND_RADAR_RANGE_BREAKBOUNDS:
      if (this->range_breakbounds_binary_sensor_ != nullptr && length >= 1) {
        this->range_breakbounds_binary_sensor_->publish_state(data[FRAME_DATA_INDEX]);
      }
      break;
    default:
      ESP_LOGE(TAG, "80 %02x %02x %02x", command, data[FRAME_DATA_INDEX + 1], data[FRAME_DATA_INDEX]);
      break;
  }
}

void MICRADARComponent::micradar_frame_parse_human_information_(uint8_t command, uint8_t *data) {
  uint16_t length = encode_uint16(data[FRAME_LENGTH_INDEX], data[FRAME_LENGTH_INDEX + 1]);
  switch (command) {
    case COMMAND_HUMAN_EXIST:
    case COMMAND_HUMAN_EXIST + 0x80:
#ifdef USE_SWITCH
      if (this->human_exist_switch_ != nullptr && length >= 1) {
        this->human_exist_switch_->publish_state(data[FRAME_DATA_INDEX]);
      }
#endif
      break;
    case COMMAND_HUMAN_TARGET:
    case COMMAND_HUMAN_TARGET + 0x80:
      if (this->has_target_binary_sensor_ != nullptr && length >= 1) {
        this->has_target_binary_sensor_->publish_state(data[FRAME_DATA_INDEX]);
      }
      break;
    case COMMAND_HUMAN_MOTION:
    case COMMAND_HUMAN_MOTION + 0x80:
      if (this->motion_status_text_sensor_ != nullptr && length >= 1) {
        this->motion_status_text_sensor_->publish_state(S_MOTION_STATUS_STR[data[FRAME_DATA_INDEX]]);
      }
      break;
    case COMMAND_HUMAN_ACTIVITY:
    case COMMAND_HUMAN_ACTIVITY + 0x80:
    if (this->movement_signs_sensor_ != nullptr && length >= 1) {
      this->movement_signs_sensor_->publish_state(data[FRAME_DATA_INDEX]);
    }
      break;
    case COMMAND_HUMAN_DISTANCE:
    case COMMAND_HUMAN_DISTANCE + 0x80:
      if (this->distance_sensor_ != nullptr && length >= 2) {
        uint16_t current_distance_int = encode_uint16(data[FRAME_DATA_INDEX], data[FRAME_DATA_INDEX + 1]);
        this->distance_sensor_->publish_state(current_distance_int);
      }
      break;
    case COMMAND_HUMAN_POSITION:
    case COMMAND_HUMAN_POSITION + 0x80:
      if (this->move_x_sensor_ != nullptr && length >= 6) {
        float move_x_float = hex_to_signed_int(&data[FRAME_DATA_INDEX], 0);
        this->move_x_sensor_->publish_state(move_x_float);
      }
      if (this->move_y_sensor_ != nullptr && length >= 6) {
        float move_y_float = hex_to_signed_int(&data[FRAME_DATA_INDEX], 2);
        this->move_y_sensor_->publish_state(move_y_float);
      }
      if (this->move_z_sensor_ != nullptr && length >= 6) {
        float move_z_float = hex_to_signed_int(&data[FRAME_DATA_INDEX], 4);
        this->move_z_sensor_->publish_state(move_z_float);
      }
      break;
    case 0x0A:
    case 0x9A:
#ifdef USE_SELECT
      if (this->unman_time_select_ != nullptr) {
        // none:0x00  1s:0x01 30s:0x02 1min:0x03 2min:0x04 5min:0x05 10min:0x06 30min:0x07 1hour:0x08
        if (data[FRAME_DATA_INDEX] < 9) {
          this->unman_time_select_->publish_state(S_UNMANNED_TIME_STR[data[FRAME_DATA_INDEX]]);
        }
      }
#endif
      break;
    case 0x0B:
    case 0x8B:
      if (this->keep_away_text_sensor_ != nullptr) {
        // none:0x00  close_to:0x01  far_away:0x02
        if (data[FRAME_DATA_INDEX] < 3) {
          this->keep_away_text_sensor_->publish_state(S_KEEP_AWAY_STR[data[FRAME_DATA_INDEX]]);
        }
      }
      break;
    default:
      ESP_LOGE(TAG, "80 %02x %02x %02x", command, data[FRAME_DATA_INDEX + 1], data[FRAME_DATA_INDEX]);
      break;
  }
}

void MICRADARComponent::micradar_frame_parse_human_breathe_information_(uint8_t command, uint8_t *data) {
  uint16_t length = encode_uint16(data[FRAME_LENGTH_INDEX], data[FRAME_LENGTH_INDEX + 1]);
  switch (command) {
    case COMMAND_HUMAN_BREATHE_MONITOR:
    case COMMAND_HUMAN_BREATHE_MONITOR + 0x80:
#ifdef USE_SWITCH
      if (this->human_breathe_switch_ != nullptr && length >= 1) {
        this->human_breathe_switch_->publish_state(data[FRAME_DATA_INDEX]);
      }
      break;
#endif
    case COMMAND_HUMAN_BREATHE_INFO:
      if (this->breathe_info_text_sensor_ != nullptr && length >= 1) {
        this->breathe_info_text_sensor_->publish_state(S_BREATHE_INFO_STR[data[FRAME_DATA_INDEX]]);
      }
      break;
    case COMMAND_HUMAN_BREATHE_RATE_NUMBER:
      if (this->breathe_rate_sensor_ != nullptr && length >= 1) {
        uint8_t current_breathe_rate_int = data[FRAME_DATA_INDEX];
        this->breathe_rate_sensor_->publish_state(current_breathe_rate_int);
      }
      break;
    case COMMAND_HUMAN_BREATHE_SLOW_LEVEL:
#ifdef USE_NUMBER
      if (this->breathe_slow_level_number_ != nullptr && length >= 1) {
        uint8_t breathe_slow_level_int = data[FRAME_DATA_INDEX];
        this->breathe_slow_level_number_->publish_state(breathe_slow_level_int);
      }
#endif
      break;
    default:
      ESP_LOGE(TAG, "81 %02x %02x %02x", command, data[FRAME_DATA_INDEX + 1], data[FRAME_DATA_INDEX]);
      break;
  }
}

void MICRADARComponent::micradar_frame_parse_human_tracking_information_(uint8_t command, uint8_t *data) {
  uint16_t length = encode_uint16(data[FRAME_LENGTH_INDEX], data[FRAME_LENGTH_INDEX + 1]);
  switch (command) {
    case COMMAND_HUMAN_TRACKING_INFO:
    case COMMAND_HUMAN_TRACKING_INFO + 0x80:
      {
        int16_t start = 0;
        int16_t val = 0;
#ifdef USE_SENSOR
        for (uint8_t index = 0; index < MAX_TARGETS; index++) {
          if ((index + 1) * 11 <= length) {
            // X
            start = 3 + index * 11;
            sensor::Sensor *sx = this->move_x_sensors_[index];
            if (sx != nullptr) {
              val = hex_to_signed_int(&data[FRAME_DATA_INDEX + start], 0);
              // tx = val;
              sx->publish_state(val);
            }
            // Y
            start = 5 + index * 11;
            sensor::Sensor *sy = this->move_y_sensors_[index];
            if (sy != nullptr) {
              val = hex_to_signed_int(&data[FRAME_DATA_INDEX + start], 0);
              // ty = val;
              sy->publish_state(val);
            }
            // Height
            start = 7 + index * 11;
            sensor::Sensor *sh = this->move_height_sensors_[index];
            if (sh != nullptr) {
              val = hex_to_signed_int(&data[FRAME_DATA_INDEX + start], 0);
              sh->publish_state(val);
            }
            // SPEED
            start = 9 + index * 11;
            val = hex_to_signed_int(&data[FRAME_DATA_INDEX + start], 0);
            //ts = val;
            //if (val) {
            //  is_moving = true;
            //  moving_target_count++;
            //}
            sensor::Sensor *ss = this->move_speed_sensors_[index];
            if (ss != nullptr) {
              ss->publish_state(val);
            }
          }

#endif
        }
      }
      break;
    default:
      ESP_LOGE(TAG, "82 %02x %02x %02x", command, data[FRAME_DATA_INDEX + 1], data[FRAME_DATA_INDEX]);
      break;
  }
}

void MICRADARComponent::micradar_frame_parse_human_sleep_information_(uint8_t command, uint8_t *data) {
  uint16_t length = encode_uint16(data[FRAME_LENGTH_INDEX], data[FRAME_LENGTH_INDEX + 1]);
  switch (command) {
    case COMMAND_HUMAN_SLEEP_MONITOR:
    case COMMAND_HUMAN_SLEEP_MONITOR + 0x80:
#ifdef USE_SWITCH
      if (this->human_sleep_monitor_switch_ != nullptr && length >= 1) {
        this->human_sleep_monitor_switch_->publish_state(data[FRAME_DATA_INDEX]);
      }
#endif
      break;
    case COMMAND_HUMAN_BED_STATUS:
    case COMMAND_HUMAN_BED_STATUS + 0x80:
      if (this->bed_status_sensor_ != nullptr && length >= 1) {
        this->bed_status_sensor_->publish_state(data[FRAME_DATA_INDEX]);
      }
      break;
    case COMMAND_HUMAN_SLEEP_STATUS:
    case COMMAND_HUMAN_SLEEP_STATUS + 0x80:
      if (this->sleep_status_text_sensor_ != nullptr && length >= 1) {
        this->sleep_status_text_sensor_->publish_state(S_SLEEP_STATUS_STR[data[FRAME_DATA_INDEX]]);
      }
      break;
    case COMMAND_HUMAN_WAKE_UP:
    case COMMAND_HUMAN_WAKE_UP + 0x80:
      if (this->wake_up_sensor_ != nullptr && length >= 2) {
        uint16_t wake_up_int = encode_uint16(data[FRAME_DATA_INDEX], data[FRAME_DATA_INDEX + 1]);
        this->wake_up_sensor_->publish_state(wake_up_int);
      }
      break;
    case COMMAND_HUMAN_LIGHT_SLEEP:
    case COMMAND_HUMAN_LIGHT_SLEEP + 0x80:
      if (this->light_sleep_sensor_ != nullptr && length >= 2) {
        uint16_t light_sleep_int = encode_uint16(data[FRAME_DATA_INDEX], data[FRAME_DATA_INDEX + 1]);
        this->light_sleep_sensor_->publish_state(light_sleep_int);
      }
      break;
    case COMMAND_HUMAN_DEEP_SLEEP:
    case COMMAND_HUMAN_DEEP_SLEEP + 0x80:
      if (this->deep_sleep_sensor_ != nullptr && length >= 2) {
        uint16_t deep_sleep_int = encode_uint16(data[FRAME_DATA_INDEX], data[FRAME_DATA_INDEX + 1]);
        this->deep_sleep_sensor_->publish_state(deep_sleep_int);
      }
      break;
    case COMMAND_HUMAN_SLEEP_QUALITY:
    case COMMAND_HUMAN_SLEEP_QUALITY + 0x80:
      if (this->sleep_quality_sensor_ != nullptr && length >= 1) {
        this->sleep_quality_sensor_->publish_state(data[FRAME_DATA_INDEX]);
      }
      break;
    case COMMAND_HUMAN_SLEEP_TOTAL_STATUS:
    case COMMAND_HUMAN_SLEEP_TOTAL_STATUS + 0x80:
      if (this->sleep_total_status_text_sensor_ != nullptr && length >= 1) {
        char buf[64];
        memset(buf, 0, 64);
        for (int i = 0; i < length; i++)
          sprintf(&buf[i * 3], "%02x ", data[FRAME_DATA_INDEX + i]);
        std::string status = buf;
        this->sleep_total_status_text_sensor_->publish_state(status);
      }
      break;
    case COMMAND_HUMAN_SLEEP_QUALITY_LEVEL:
    case COMMAND_HUMAN_SLEEP_QUALITY_LEVEL + 0x80:
      if (this->sleep_quality_level_text_sensor_ != nullptr && length >= 1) {
        this->sleep_quality_level_text_sensor_->publish_state(S_SLEEP_QUALITY_LEVEL_STR[data[FRAME_DATA_INDEX]]);
      }
      break;

      case COMMAND_HUMAN_SLEEP_ABSENCE:
      case COMMAND_HUMAN_SLEEP_ABSENCE + 0x80:
  #ifdef USE_SWITCH
        if (this->human_sleep_absence_switch_ != nullptr && length >= 1) {
          this->human_sleep_absence_switch_->publish_state(data[FRAME_DATA_INDEX]);
        }
  #endif
        break;
    case COMMAND_HUMAN_SLEEP_ROLL_OVER:
    case COMMAND_HUMAN_SLEEP_ROLL_OVER + 0x80:
#ifdef USE_SELECT
      if ((this->sleep_roll_over_select_ != nullptr) &&
             (this->sleep_roll_over_select_->has_index(data[FRAME_DATA_INDEX]))) {
        this->sleep_roll_over_select_->publish_state(S_SLEEP_ROLLOVER_STR[data[FRAME_DATA_INDEX]]);
      }
#endif
      break;
    default:
      ESP_LOGE(TAG, "84 %02x %02x %02x", command, data[FRAME_DATA_INDEX + 1], data[FRAME_DATA_INDEX]);
      break;
  }
}

void MICRADARComponent::micradar_frame_parse_human_heart_rate_information_(uint8_t command, uint8_t *data) {
  uint16_t length = encode_uint16(data[FRAME_LENGTH_INDEX], data[FRAME_LENGTH_INDEX + 1]);
  switch (command) {
    case COMMAND_HUMAN_HEART_RATE_MONITOR:
    case COMMAND_HUMAN_HEART_RATE_MONITOR + 0x80:
#ifdef USE_SWITCH
      if (this->human_heart_monitor_switch_ != nullptr && length >= 1) {
        this->human_heart_monitor_switch_->publish_state(data[FRAME_DATA_INDEX]);
      }
#endif
      break;
    case COMMAND_HUMAN_HEART_RATE_NUMBER:
    case COMMAND_HUMAN_HEART_RATE_NUMBER + 0x80:
      if (this->heartbeat_rate_sensor_ != nullptr && length >= 1) {
        uint8_t current_heartbeat_rate_int = data[FRAME_DATA_INDEX];
        this->heartbeat_rate_sensor_->publish_state(current_heartbeat_rate_int);
      }
      break;
    case COMMAND_HUMAN_HEART_WAVE + 0x80:
#ifdef USE_SWITCH
      if (this->human_heart_wave_switch_ != nullptr && length >= 1) {
        this->human_heart_wave_switch_->publish_state(data[FRAME_DATA_INDEX]);
      }
#endif
      break;
    default:
      ESP_LOGE(TAG, "85 %02x %02x %02x", command, data[FRAME_DATA_INDEX + 1], data[FRAME_DATA_INDEX]);
      break;
  }
}

void MICRADARComponent::micradar_frame_parse_human_count_information_(uint8_t command, uint8_t *data) {
  uint16_t length = encode_uint16(data[FRAME_LENGTH_INDEX], data[FRAME_LENGTH_INDEX + 1]);
  switch (command) {
    default:
      ESP_LOGE(TAG, "86 %02x %02x %02x", command, data[FRAME_DATA_INDEX + 1], data[FRAME_DATA_INDEX]);
      break;
  }
}

void MICRADARComponent::send_query(uint8_t control, uint8_t command, uint8_t value) {
  uint8_t send_data[10] = {
    FRAME_HEADER1_VALUE, FRAME_HEADER2_VALUE, control, command,
    0x00, 0x01, value, 0x00,
    FRAME_TAIL1_VALUE, FRAME_TAIL2_VALUE
  };
  send_data[7] = get_frame_crc_sum(send_data, 10);

  ESP_LOGV(TAG, "send query %02x %02x %02x %02x", control, command, value, send_data[7]);
  this->send_query_(send_data, 10);
}

// Sending data frames
void MICRADARComponent::send_query_(const uint8_t *query, size_t string_length) {
  this->write_array(query, string_length);
  delay(50);
}

// Is the function supported
bool MICRADARComponent::is_function_supported(uint8_t control, uint8_t command) {

    if (!this->model_.compare("R60AMP1")) {
      if ((control == CONTROL_MAIN) && (command == COMMAND_HEARTBEAT))
        return false;
      if ((control == CONTROL_HUMAN_INFORMATION) && (command == 0x8B))
        return false;
      if ((control == CONTROL_HUMAN_INFORMATION) && (command == 0xBA))
        return false;
      if ((control == CONTROL_WORK) && (command == 0x88))
        return false;
      if ((control == CONTROL_WORK) && (command == 0x89))
        return false;
    }

    if (!this->model_.compare("R60ABD1")) {
      if ((control == CONTROL_HUMAN_INFORMATION) && (command == 0x8B))
        return false;
      if ((control == CONTROL_HUMAN_INFORMATION) && (command == 0xBA))
        return false;
    }

    return true;
}

bool MICRADARComponent::is_function_supported(uint8_t function) {

  switch (function) {
    case STANDARD_FUNCTION_QUERY_SCENE_MODE:
      return is_function_supported(CONTROL_WORK, 0x87);
    case STANDARD_FUNCTION_QUERY_SENSITIVITY:
      return is_function_supported(CONTROL_WORK, 0x88);
    case STANDARD_FUNCTION_QUERY_UNMANNED_TIME:
      return is_function_supported(CONTROL_HUMAN_INFORMATION, 0xBA);
    case STANDARD_FUNCTION_QUERY_HUMAN_STATUS:
      return is_function_supported(CONTROL_HUMAN_INFORMATION, 0x81);
    case STANDARD_FUNCTION_QUERY_HUMAN_MOTION_INF:
      return is_function_supported(CONTROL_HUMAN_INFORMATION, 0x82);
    case STANDARD_FUNCTION_QUERY_BODY_MOVE_PARAMETER:
      return is_function_supported(CONTROL_HUMAN_INFORMATION, 0x83);
    case STANDARD_FUNCTION_QUERY_KEEPAWAY_STATUS:
      return is_function_supported(CONTROL_HUMAN_INFORMATION, 0x8B);
    case STANDARD_FUNCTION_QUERY_CUSTOM_MODE:
      return is_function_supported(CONTROL_WORK, 0x89);
    case STANDARD_FUNCTION_QUERY_HEARTBEAT_STATE:
      return is_function_supported(CONTROL_MAIN, COMMAND_HEARTBEAT);
    case STANDARD_FUNCTION_QUERY_SLEEP_STATE:
      return is_function_supported(CONTROL_HUMAN_SLEEP_INFORMATION, COMMAND_HUMAN_BED_STATUS);
    default:
      break;
  }
  return true;
}

// Send Heartbeat Packet Command
void MICRADARComponent::get_heartbeat_packet() {
  this->send_query(CONTROL_HUMAN_HEART_RATE_TYPE_INFORMATION, COMMAND_HUMAN_HEART_RATE_MONITOR + 0x80, 0x0F);
  this->send_query_(GET_HEARTBEAT, sizeof(GET_HEARTBEAT));
}

// Send Sleep Packet Command
void MICRADARComponent::get_sleep_packet() {
  this->send_query(CONTROL_HUMAN_SLEEP_INFORMATION, COMMAND_HUMAN_SLEEP_MONITOR + 0x80, 0x0F);
  this->send_query_(GET_SLEEP_QUALITY, sizeof(GET_SLEEP_QUALITY));
  this->send_query_(GET_BED_STATUS, sizeof(GET_BED_STATUS));
  this->get_sleep_rollover();
}

void MICRADARComponent::get_sleep_rollover() {
  this->send_query(CONTROL_HUMAN_SLEEP_INFORMATION, COMMAND_HUMAN_SLEEP_ROLL_OVER + 0x80, 0x0F);
}

// Send Breathe Packet Command
void MICRADARComponent::get_breathe_packet() {
  this->send_query(CONTROL_HUMAN_BREATHE_INFORMATION, COMMAND_HUMAN_HEART_RATE_MONITOR + 0x80, 0x0F);
}

// Issuance of the underlying open parameter query command
void MICRADARComponent::get_radar_output_information_switch() {
  this->send_query_(GET_RADAR_OUTPUT_INFORMATION_SWITCH, sizeof(GET_RADAR_OUTPUT_INFORMATION_SWITCH));
}

// Issuance of product model orders
void MICRADARComponent::get_product_mode() {
  this->send_query_(GET_PRODUCT_MODE, sizeof(GET_PRODUCT_MODE));
}

// Issuing the Get Product ID command
void MICRADARComponent::get_product_id() {
  this->send_query_(GET_PRODUCT_ID, sizeof(GET_PRODUCT_ID));
}

// Issuing hardware model commands
void MICRADARComponent::get_hardware_model() {
  this->send_query_(GET_HARDWARE_MODEL, sizeof(GET_HARDWARE_MODEL));
}

// Issuing software version commands
void MICRADARComponent::get_firmware_version() {
  this->send_query_(GET_FIRMWARE_VERSION, sizeof(GET_FIRMWARE_VERSION));
}

void MICRADARComponent::get_human_status() {
  this->send_query(CONTROL_HUMAN_INFORMATION, COMMAND_HUMAN_EXIST + 0x80, 0x0F);
  this->send_query_(GET_HUMAN_STATUS, sizeof(GET_HUMAN_STATUS));
}

void MICRADARComponent::get_human_motion_info() {
  this->send_query_(GET_HUMAN_MOTION_INFORMATION, sizeof(GET_HUMAN_MOTION_INFORMATION));
}

void MICRADARComponent::get_body_motion_params() {
  this->send_query_(GET_BODY_MOTION_PARAMETERS, sizeof(GET_BODY_MOTION_PARAMETERS));
}

void MICRADARComponent::get_keep_away() {
  this->send_query_(GET_KEEP_AWAY, sizeof(GET_KEEP_AWAY));
}

void MICRADARComponent::get_scene_mode() {
  this->send_query_(GET_SCENE_MODE, sizeof(GET_SCENE_MODE));
 }

void MICRADARComponent::get_sensitivity() {
  this->send_query_(GET_SENSITIVITY, sizeof(GET_SENSITIVITY));
}

void MICRADARComponent::get_unmanned_time() {
  this->send_query_(GET_UNMANNED_TIME, sizeof(GET_UNMANNED_TIME));
}

void MICRADARComponent::get_custom_mode() {
  this->send_query_(GET_CUSTOM_MODE, sizeof(GET_CUSTOM_MODE));
}

void MICRADARComponent::get_existence_boundary() {
  this->send_query_(GET_EXISTENCE_BOUNDARY, sizeof(GET_EXISTENCE_BOUNDARY));
}

void MICRADARComponent::get_motion_boundary() {
  this->send_query_(GET_MOTION_BOUNDARY, sizeof(GET_MOTION_BOUNDARY));
}

void MICRADARComponent::get_spatial_static_value() {
  this->send_query_(GET_SPATIAL_STATIC_VALUE, sizeof(GET_SPATIAL_STATIC_VALUE));
}

void MICRADARComponent::get_spatial_motion_value() {
  this->send_query_(GET_SPATIAL_MOTION_VALUE, sizeof(GET_SPATIAL_MOTION_VALUE));
}

void MICRADARComponent::get_distance_of_static_object() {
  this->send_query_(GET_DISTANCE_OF_STATIC_OBJECT, sizeof(GET_DISTANCE_OF_STATIC_OBJECT));
}

void MICRADARComponent::get_distance_of_moving_object() {
  this->send_query_(GET_DISTANCE_OF_MOVING_OBJECT, sizeof(GET_DISTANCE_OF_MOVING_OBJECT));
}

void MICRADARComponent::get_target_movement_speed() {
  this->send_query_(GET_TARGET_MOVEMENT_SPEED, sizeof(GET_TARGET_MOVEMENT_SPEED));
}

void MICRADARComponent::get_existence_threshold() {
  this->send_query_(GET_EXISTENCE_THRESHOLD, sizeof(GET_EXISTENCE_THRESHOLD));
}

void MICRADARComponent::get_motion_threshold() {
  this->send_query_(GET_MOTION_THRESHOLD, sizeof(GET_MOTION_THRESHOLD));
}

void MICRADARComponent::get_motion_trigger_time() {
  this->send_query_(GET_MOTION_TRIGGER_TIME, sizeof(GET_MOTION_TRIGGER_TIME));
}

void MICRADARComponent::get_motion_to_rest_time() {
  this->send_query_(GET_MOTION_TO_REST_TIME, sizeof(GET_MOTION_TO_REST_TIME));
}

void MICRADARComponent::get_custom_unman_time() {
  this->send_query_(GET_CUSTOM_UNMAN_TIME, sizeof(GET_CUSTOM_UNMAN_TIME));
}

// Logic of setting: After setting, query whether the setting is successful or not!

void MICRADARComponent::set_underlying_open_function(bool enable) {
  if (enable) {
    this->send_query_(UNDERLYING_SWITCH_ON, sizeof(UNDERLYING_SWITCH_ON));
  } else {
    this->send_query_(UNDERLYING_SWITCH_OFF, sizeof(UNDERLYING_SWITCH_OFF));
  }
  if (this->keep_away_text_sensor_ != nullptr) {
    this->keep_away_text_sensor_->publish_state("");
  }
  if (this->motion_status_text_sensor_ != nullptr) {
    this->motion_status_text_sensor_->publish_state("");
  }
  if (this->custom_spatial_static_value_sensor_ != nullptr) {
    this->custom_spatial_static_value_sensor_->publish_state(NAN);
  }
  if (this->custom_spatial_motion_value_sensor_ != nullptr) {
    this->custom_spatial_motion_value_sensor_->publish_state(NAN);
  }
  if (this->custom_motion_distance_sensor_ != nullptr) {
    this->custom_motion_distance_sensor_->publish_state(NAN);
  }
  if (this->custom_presence_of_detection_sensor_ != nullptr) {
    this->custom_presence_of_detection_sensor_->publish_state(NAN);
  }
  if (this->custom_motion_speed_sensor_ != nullptr) {
    this->custom_motion_speed_sensor_->publish_state(NAN);
  }
}

void MICRADARComponent::set_switch(uint8_t control, uint8_t command, bool enable) {
  uint8_t send_data[10] = {
    FRAME_HEADER1_VALUE, FRAME_HEADER2_VALUE, control, command,
    0x00, 0x01, 0x01, 0x00,
    FRAME_TAIL1_VALUE, FRAME_TAIL2_VALUE
  };
  if (enable) {
    send_data[6] = 0x01;
  } else {
    send_data[6] = 0x00;
  }
  send_data[7] = get_frame_crc_sum(send_data, 10);
  this->send_query_(send_data, 10);
}

void MICRADARComponent::send_byte(uint8_t control, uint8_t command, uint8_t value) {
  uint8_t send_data[10] = {
    FRAME_HEADER1_VALUE, FRAME_HEADER2_VALUE, control, command,
    0x00, 0x01, value, 0x00,
    FRAME_TAIL1_VALUE, FRAME_TAIL2_VALUE
  };
  send_data[7] = get_frame_crc_sum(send_data, 10);
  this->send_query_(send_data, 10);
}

void MICRADARComponent::send_word(uint8_t control, uint8_t command, uint16_t value) {
  uint8_t send_data[11] = {
    FRAME_HEADER1_VALUE, FRAME_HEADER2_VALUE, control, command,
    0x00, 0x04, 0x00, 0x00, 0x00,
    FRAME_TAIL1_VALUE, FRAME_TAIL2_VALUE
  };

  send_data[6] = (value >> 8) & 0xff;
  send_data[7] = value & 0xff;
  send_data[8] = get_frame_crc_sum(send_data, 11);
  this->send_query_(send_data, 11);
}

void MICRADARComponent::send_dword(uint8_t control, uint8_t command, uint32_t value) {
  uint8_t send_data[13] = {
    FRAME_HEADER1_VALUE, FRAME_HEADER2_VALUE, control, command,
    0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
    FRAME_TAIL1_VALUE, FRAME_TAIL2_VALUE
  };

  send_data[6] = (value >> 24) & 0xff;
  send_data[7] = (value >> 16) & 0xff;
  send_data[8] = (value >> 8) & 0xff;
  send_data[9] = value & 0xff;
  send_data[10] = get_frame_crc_sum(send_data, 13);
  this->send_query_(send_data, 13);
}

void MICRADARComponent::set_human_exist(bool enable) {
  this->set_switch(CONTROL_HUMAN_INFORMATION, 0x00, enable);
}

void MICRADARComponent::set_human_breathe(bool enable) {
  this->set_switch(CONTROL_HUMAN_BREATHE_INFORMATION, 0x00, enable);
}

void MICRADARComponent::set_human_heart_monitor(bool enable) {
  this->set_switch(CONTROL_HUMAN_HEART_RATE_TYPE_INFORMATION, 0x00, enable);
}

void MICRADARComponent::set_human_heart_wave(bool enable) {
  this->set_switch(CONTROL_HUMAN_HEART_RATE_TYPE_INFORMATION, 0x8A, enable);
}

void MICRADARComponent::set_human_sleep_monitor(bool enable) {
  this->set_switch(CONTROL_HUMAN_SLEEP_INFORMATION, 0x00, enable);
}

void MICRADARComponent::set_human_sleep_absence(bool enable) {
  this->set_switch(CONTROL_HUMAN_SLEEP_INFORMATION, 0x14, enable);
}

void MICRADARComponent::set_scene_mode(uint8_t value) {
  this->send_byte(CONTROL_WORK, 0x07, value);
#ifdef USE_NUMBER
  if (this->custom_mode_number_ != nullptr) {
    this->custom_mode_number_->publish_state(0);
  }
#endif
  if (this->custom_mode_num_sensor_ != nullptr) {
    this->custom_mode_num_sensor_->publish_state(0);
  }
  this->get_scene_mode();
  this->get_sensitivity();
  this->get_custom_mode();
  this->get_existence_boundary();
  this->get_motion_boundary();
  this->get_existence_threshold();
  this->get_motion_threshold();
  this->get_motion_trigger_time();
  this->get_motion_to_rest_time();
  this->get_custom_unman_time();
}

void MICRADARComponent::set_sensitivity(uint8_t value) {
  if (value == 0x00)
    return;
  this->send_byte(CONTROL_WORK, 0x08, value);
  this->get_scene_mode();
  this->get_sensitivity();
}

void MICRADARComponent::set_restart() {
  this->send_query_(SET_RESTART, sizeof(SET_RESTART));
  this->check_dev_inf_sign_ = true;
}

void MICRADARComponent::set_unman_time(uint8_t value) {
  this->send_byte(CONTROL_HUMAN_INFORMATION, 0x0A, value);
  this->get_unmanned_time();
}

void MICRADARComponent::set_custom_mode(uint8_t mode) {
  if (mode == 0) {
    this->set_custom_end_mode();  // Equivalent to end setting
#ifdef USE_NUMBER
    if (this->custom_mode_number_ != nullptr) {
      this->custom_mode_number_->publish_state(0);
    }
#endif
    return;
  }
  this->send_byte(CONTROL_HUMAN_INFORMATION, 0x09, mode);

  this->get_existence_boundary();
  this->get_motion_boundary();
  this->get_existence_threshold();
  this->get_motion_threshold();
  this->get_motion_trigger_time();
  this->get_motion_to_rest_time();
  this->get_custom_unman_time();
  this->get_custom_mode();
  this->get_scene_mode();
  this->get_sensitivity();
}

void MICRADARComponent::set_custom_end_mode() {
  this->send_byte(CONTROL_WORK, 0x0A, 0x0F);
#ifdef USE_NUMBER
  if (this->custom_mode_number_ != nullptr) {
    this->custom_mode_number_->publish_state(0);  // Clear setpoints
  }
#endif
  this->get_existence_boundary();
  this->get_motion_boundary();
  this->get_existence_threshold();
  this->get_motion_threshold();
  this->get_motion_trigger_time();
  this->get_motion_to_rest_time();
  this->get_custom_unman_time();
  this->get_custom_mode();
  this->get_scene_mode();
  this->get_sensitivity();
}

void MICRADARComponent::set_existence_boundary(uint8_t value) {
  if ((this->custom_mode_num_sensor_ != nullptr) && (this->custom_mode_num_sensor_->state == 0))
    return;  // You'll have to check that you're in custom mode to set it up

  this->send_byte(0x08, 0x0A, (uint8_t) (value + 1));
  this->get_existence_boundary();
}

void MICRADARComponent::set_motion_boundary(uint8_t value) {
  if ((this->custom_mode_num_sensor_ != nullptr) && (this->custom_mode_num_sensor_->state == 0))
    return;  // You'll have to check that you're in custom mode to set it up

  this->send_byte(0x08, 0x0B, (uint8_t) (value + 1));
  this->get_motion_boundary();
}

void MICRADARComponent::set_sleep_rollover(uint8_t value) {
  this->send_byte(CONTROL_HUMAN_SLEEP_INFORMATION, COMMAND_HUMAN_SLEEP_ROLL_OVER, value);
  this->get_sleep_rollover();
}

void MICRADARComponent::set_existence_threshold(uint8_t value) {
  if ((this->custom_mode_num_sensor_ != nullptr) && (this->custom_mode_num_sensor_->state == 0))
    return;  // You'll have to check that you're in custom mode to set it up

  this->send_byte(0x08, 0x08, value);
  this->get_existence_threshold();
}

void MICRADARComponent::set_motion_threshold(uint8_t value) {
  if ((this->custom_mode_num_sensor_ != nullptr) && (this->custom_mode_num_sensor_->state == 0))
    return;  // You'll have to check that you're in custom mode to set it up

  this->send_byte(0x08, 0x09, value);
  this->get_motion_threshold();
}

void MICRADARComponent::set_motion_trigger_time(uint8_t value) {
  if ((this->custom_mode_num_sensor_ != nullptr) && (this->custom_mode_num_sensor_->state == 0))
    return;  // You'll have to check that you're in custom mode to set it up
  this->send_dword(0x08, 0x0C, (uint16_t) value);
  this->get_motion_trigger_time();
}

void MICRADARComponent::set_motion_to_rest_time(uint16_t value) {
  if ((this->custom_mode_num_sensor_ != nullptr) && (this->custom_mode_num_sensor_->state == 0))
    return;  // You'll have to check that you're in custom mode to set it up
  uint8_t h8_num = (value >> 8) & 0xff;
  uint8_t l8_num = value & 0xff;
  uint8_t send_data[13] = {0x53, 0x59, 0x08, 0x0D, 0x00, 0x04, 0x00, 0x00, h8_num, l8_num, 0x00, 0x54, 0x43};
  send_data[10] = get_frame_crc_sum(send_data, 10);
  this->send_query_(send_data, 13);
  this->get_motion_to_rest_time();
}

void MICRADARComponent::set_custom_unman_time(uint16_t value) {
  if ((this->custom_mode_num_sensor_ != nullptr) && (this->custom_mode_num_sensor_->state == 0))
    return;  // You'll have to check that you're in custom mode to set it up
  uint32_t value_ms = value * 1000;
  uint8_t h24_num = (value_ms >> 24) & 0xff;
  uint8_t h16_num = (value_ms >> 16) & 0xff;
  uint8_t h8_num = (value_ms >> 8) & 0xff;
  uint8_t l8_num = value_ms & 0xff;
  uint8_t send_data[13] = {0x53, 0x59, 0x08, 0x0E, 0x00, 0x04, h24_num, h16_num, h8_num, l8_num, 0x00, 0x54, 0x43};
  send_data[10] = get_frame_crc_sum(send_data, 10);
  this->send_query_(send_data, 13);
  this->get_custom_unman_time();
}

void MICRADARComponent::set_breathe_slow_level(uint8_t value) {
  this->send_word(CONTROL_HUMAN_BREATHE_INFORMATION, COMMAND_HUMAN_BREATHE_SLOW_LEVEL, (uint8_t) value);
}

void MICRADARComponent::set_sleep_absence_time(uint8_t value) {
  this->send_word(CONTROL_HUMAN_SLEEP_INFORMATION, COMMAND_HUMAN_SLEEP_ABSENCE_TIME, (uint8_t) value);
}

void MICRADARComponent::set_sleep_end_time(uint8_t value) {
  this->send_word(CONTROL_HUMAN_SLEEP_INFORMATION, COMMAND_HUMAN_SLEEP_END_TIME, (uint8_t) value);
}

#ifdef USE_SENSOR
void MICRADARComponent::set_move_x_sensor(uint8_t target, sensor::Sensor *s) { this->move_x_sensors_[target] = s; }
void MICRADARComponent::set_move_y_sensor(uint8_t target, sensor::Sensor *s) { this->move_y_sensors_[target] = s; }
void MICRADARComponent::set_move_speed_sensor(uint8_t target, sensor::Sensor *s) {
  this->move_speed_sensors_[target] = s;
}
void MICRADARComponent::set_move_height_sensor(uint8_t target, sensor::Sensor *s) {
  this->move_height_sensors_[target] = s;
}
void MICRADARComponent::set_move_distance_sensor(uint8_t target, sensor::Sensor *s) {
  this->move_distance_sensors_[target] = s;
}
void MICRADARComponent::set_move_resolution_sensor(uint8_t target, sensor::Sensor *s) {
  this->move_resolution_sensors_[target] = s;
}
#endif

void MICRADARComponent::get_radar_parameters() {
  this->get_hardware_model();

  if (this->is_function_supported(STANDARD_FUNCTION_QUERY_SLEEP_STATE)) {
    this->get_sleep_packet();
  }

  if (this->is_function_supported(STANDARD_FUNCTION_QUERY_HUMAN_STATUS))
    this->get_human_status();
}

void MICRADARComponent::factory_reset() {
  uint8_t send_data[10] = {
    FRAME_HEADER1_VALUE, FRAME_HEADER2_VALUE, CONTROL_MAIN, COMMAND_RESTART,
    0x00, 0x01, 0x0F, 0xBD,
    FRAME_TAIL1_VALUE, FRAME_TAIL2_VALUE
  };
  send_data[7] = get_frame_crc_sum(send_data, 7);
  this->write_array(send_data, 10);
  this->get_radar_parameters();
}

}  // namespace micradar
}  // namespace esphome

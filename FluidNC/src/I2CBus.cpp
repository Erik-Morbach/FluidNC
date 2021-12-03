#include "I2CBus.h"

void I2CBus::validate() const{
    Assert(_sda.defined(), "I2C SDA pin should be configured once");
    Assert(_scl.defined(), "I2C SCL pin should be configured once");
}
void I2CBus::group(Configuration::HandlerBase& handler) {
    handler.item("sda",this->_sda);
    handler.item("scl",this->_scl);
    handler.item("frequency",this->_freq_hz);
}
void I2CBus::afterParse() {
    this->init_i2c_bus();
    this->init();
}

void I2CBus::init(){
    log_info("I2C SDA:" << _sda.name() << " SCL:" << _scl.name() << " Clock:" << _freq_hz);
}


void I2CBus::init_i2c_bus(){
    this->_config = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = (gpio_num_t) this->_sda.getNative(this->_sda.capabilities()),
        .sda_pullup_en = GPIO_PULLUP_DISABLE,
        .scl_io_num = (gpio_num_t) this->_scl.getNative(this->_scl.capabilities()),
        .scl_pullup_en = GPIO_PULLUP_DISABLE,
    };
    this->_config.master = {this->_freq_hz};
    Assert(i2c_param_config(this->_port_num,&this->_config)==ESP_OK,"I2C Error on config");
    Assert(i2c_driver_install(this->_port_num,this->_config.mode,0,0,0)==ESP_OK,"I2C Error Installing Driver");
}

i2c_cmd_handle_t I2CBus::begin_cmd(uint8_t device_address, uint8_t resource_address){
    auto cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd,(device_address<<1)|I2C_MASTER_WRITE,1);
    i2c_master_write_byte(cmd,resource_address,1);
    return cmd;
}

esp_err_t I2CBus::end_cmd(i2c_cmd_handle_t &cmd, uint32_t timeout, bool delete_cmd){
    i2c_master_stop(cmd);
    auto error = i2c_master_cmd_begin(this->_port_num,cmd,pdMS_TO_TICKS(timeout));
    if(delete_cmd) i2c_cmd_link_delete(cmd);
    return error;
}

esp_err_t I2CBus::write(uint8_t device_address, uint8_t resource_address, const uint8_t *data, uint8_t data_size, uint32_t timeout){
    uint8_t *_send_data = new uint8_t[data_size];
    memcpy(_send_data,data,data_size);
    auto cmd = this->begin_cmd(device_address,resource_address);
    i2c_master_write(cmd,_send_data,data_size,1);
    esp_err_t error = this->end_cmd(cmd,timeout);
    if(error==ESP_ERR_TIMEOUT) {
        i2c_reset_rx_fifo(this->_port_num);
        i2c_reset_tx_fifo(this->_port_num);
    }
    delete _send_data;
    return error;
}

esp_err_t I2CBus::read(uint8_t device_address, uint8_t resource_address, uint8_t *data, uint8_t data_size,uint32_t timeout){
    auto cmd = this->begin_cmd(device_address,resource_address); 
    esp_err_t error = this->end_cmd(cmd,timeout);
    if(error!=ESP_OK)
        return error;
    cmd = i2c_cmd_link_create(); 
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd,(device_address<<1)|I2C_MASTER_READ,1);
    i2c_master_read(cmd,data,data_size,I2C_MASTER_NACK);
    return this->end_cmd(cmd,timeout);
}


void I2CBus::end_i2c_bus(){
    Assert(i2c_driver_delete(this->_port_num)==ESP_OK,"I2C Error deleting driver");
    Assert(i2c_reset_rx_fifo(this->_port_num)==ESP_OK,"I2C Error reseting rx fifo");
    Assert(i2c_reset_tx_fifo(this->_port_num)==ESP_OK,"I2C Error reseting tx fifo");
}

I2CBus::~I2CBus(){
    this->end_i2c_bus();
}
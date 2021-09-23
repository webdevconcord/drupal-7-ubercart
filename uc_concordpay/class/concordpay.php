<?php

class concordpay
{
    const ORDER_NEW      = 'New';
    const ORDER_DECLINED = 'Declined';
    const ORDER_REFUNDED = 'Refunded';
    const ORDER_EXPIRED  = 'Expired';
    const ORDER_PENDING  = 'Pending';
    const ORDER_APPROVED = 'Approved';

    const ORDER_WAITING_AUTH_COMPLETE = 'WaitingAuthComplete';
    const ORDER_REFUND_IN_PROCESSING  = 'RefundInProcessing';
    const ORDER_IN_PROCESSING         = 'InProcessing';

    const ORDER_STATUS_COMPLETED = 'completed';
    const ORDER_STATUS_DECLINED  = 'canceled';
    const ORDER_STATUS_CANCELED  = 'canceled';

    const ORDER_SEPARATOR     = '#';
    const SIGNATURE_SEPARATOR = ';';
    const CURRENCY_UAH        = 'UAH';

    const RESPONSE_TYPE_PAYMENT = 'payment';
    const RESPONSE_TYPE_REVERSE = 'reverse';

    const URL = "https://pay.concord.ua/api/";

    /** @var string */
    protected $secret_key = '';

    /** @var string */
    protected $module_id = '';

    /**
     * @param string $module_id
     */
    public function __construct($module_id = 'ns_concordpay'){
        $this->module_id = $module_id;
    }

    /**
     * @var string[]
     */
    protected $keysForResponseSignature = array(
        'merchantAccount',
        'orderReference',
        'amount',
        'currency'
    );

    /**
     * @var string[]
     */
    protected $keysForSignature = array(
        'merchant_id',
        'order_id',
        'amount',
        'currency_iso',
        'description'
    );

    /**
    * @var string[]
    */
    protected $operationTypes = array(
      'payment',
      'reverse'
    );

    /**
     * @param $option
     * @param $keys
     * @return string
     */
    public function getSignature($option, $keys)
    {
        $hash = array();
        foreach ($keys as $dataKey) {
            if (!isset($option[$dataKey])) {
                $option[$dataKey] = '';
            }
            if (is_array($option[$dataKey])) {
                foreach ($option[$dataKey] as $v) {
                    $hash[] = $v;
                }
            } else {
                $hash [] = $option[$dataKey];
            }
        }
        $hash = implode(self::SIGNATURE_SEPARATOR, $hash);
        return hash_hmac('md5', $hash, $this->getSecretKey());
    }

    /**
     * @param $options
     * @return string
     */
    public function getRequestSignature($options)
    {
        return $this->getSignature($options, $this->keysForSignature);
    }

    /**
     * @param $options
     * @return string
     */
    public function getResponseSignature($options)
    {
        return $this->getSignature($options, $this->keysForResponseSignature);
    }

    /**
     * @param $response
     * @return bool|string
     */
    public function isPaymentValid($response)
    {
        $sign = $this->getResponseSignature($response);
        if ($sign !== $response['merchantSignature']) {
            return t('Error: Wrong payment signature');
        }

        if ($response['transactionStatus'] === self::ORDER_APPROVED) {
            return true;
        }

        return false;
    }

    /**
     * @return mixed|null
     */
    public function getSecretKey()
    {
        $secret_key = variable_get('cp_secret_key', '');

        return $secret_key;
    }

  /**
   * Get allowed operation types.
   *
   * @return string[]
   */
    public function getOperationTypes()
    {
        return $this->operationTypes;
    }
}


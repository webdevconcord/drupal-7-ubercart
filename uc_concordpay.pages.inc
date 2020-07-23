<?php
/**
 * @file
 * Page callbacks for Concordpay.
 */

/**
 * Callback function return page.
 */
function uc_concordpay_page_callback()
{

  $data = json_decode(file_get_contents("php://input"), TRUE);
  if (empty($data)) {
    drupal_goto('<front>');
  } else {
    $cp = new Concordpay();
    list($orderId,) = explode(concordpay::ORDER_SEPARATOR, $data['orderReference']);
    $order = uc_order_load($orderId);
    $paymentInfo = $cp->isPaymentValid($data);
    if($data['transactionStatus'] === concordpay::ORDER_DECLINED) {
      uc_order_update_status($order->order_id, 'canceled');
    }
    if ($paymentInfo) {
      uc_order_update_status($order->order_id, 'completed');
    }

    return http_response_code(200);
  }
}

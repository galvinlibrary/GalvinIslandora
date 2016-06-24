<?php

/**
 * @file
 * Utility functions.
 */

/**
 * Determine if the given item exists in the queue, independent of state.
 *
 * @param string $id
 *   A string representing the ID of an item in the queue.
 *
 * @return bool
 *   TRUE if there's an entry for the given ID in the queue; otherwise, FALSE.
 */
function islandora_batch_item_exists($id) {
  $result = db_select('islandora_batch_queue', 'q')
    ->fields('q', array())
    ->condition('id', $id)
    ->countQuery()
    ->execute()
    ->fetchField();
  return $result > 0;
}

/**
 * Determine if an item was created by the given user.
 *
 * @param string $id
 *   A string representing the ID of an item in the queue.
 * @param object $user
 *   An object containing a loaded Drupal user. Defaults to the global $user,
 *   if not provided.
 *
 * @return bool
 *   TRUE if the item is in a set owned by the given user; otherwise, FALSE.
 */
function islandora_batch_item_is_owner($id, $user = NULL) {
  if ($user === NULL) {
    global $user;
  }

  $q = db_select('islandora_batch_queue', 'q');
  $q->join('islandora_batch_set', 's', 's.id = q.sid');

  $result = $q->fields('q', 'id')
    ->condition('q.id', $id)
    ->condition('s.uid', $user->uid)
    ->countQuery()
    ->execute()
    ->fetchField();
  return $result > 0;
}

/**
 * Determine if a set with the given ID exists.
 *
 * @param int $id
 *   An integer identifying a batch set.
 *
 * @return bool
 *   TRUE if the batch exists; otherwise, FALSE.
 */
function islandora_batch_set_exists($id) {
  $result = db_select('islandora_batch_set', 's')
    ->fields('s', array())
    ->condition('id', $id)
    ->countQuery()
    ->execute()
    ->fetchField();

  return $result > 0;
}

/**
 * Determine if the set exists and belongs to the given user.
 *
 * @param int $id
 *   An integer identifying a batch set.
 * @param object $user
 *   An object containing a loaded Drupal user. Defaults to the global $user,
 *   if not provided.
 *
 * @return bool
 *   TRUE if the batch exists was created by the given user; otherwise, FALSE.
 */
function islandora_batch_set_is_owner($id, $user = NULL) {
  if ($user === NULL) {
    global $user;
  }

  $result = db_select('islandora_batch_set', 's')
    ->fields('s', array())
    ->condition('id', $id)
    ->condition('uid', $user->uid)
    ->countQuery()
    ->execute()
    ->fetchField();

  return $result > 0;
}

/**
 * Gets the batch ID for an item based upon PID.
 *
 * @param string $pid
 *   The PID of item in the queue.
 *
 * @return int
 *   The batch ID associated to the item in the queue.
 */
function islandora_batch_get_bid($pid) {
  $bid = db_select('islandora_batch_queue', 'q')
    ->fields('q', array('bid'))
    ->condition('id', $pid)
    ->execute()
    ->fetchField();
  return intval($bid);
}

/**
 * Cleans up and flags files that have been processed as ready to delete.
 *
 * @param string $pid
 *   The PID of the item to be cleaned up.
 */
function islandora_batch_remove_file_usages($pid) {
  $bid = islandora_batch_get_bid($pid);
  $q = db_select('file_usage', 'u')
    ->fields('u', array('fid'))
    ->condition('id', $bid)
    ->condition('module', 'islandora_batch')
    ->condition('type', 'islandora_batch_object')
    ->execute();
  $files = file_load_multiple($q->fetchCol());
  foreach ($files as $file) {
    file_usage_delete($file, 'islandora_batch', 'islandora_batch_object', $bid);
  }
}

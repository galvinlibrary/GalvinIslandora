<?php

/**
 * @file
 * This file documents all available hook functions to manipulate data.
 */

/**
 * A hook to allow for events directly after a islandora batch has finished.
 *
 * @param array $sets
 *   An array of batch set ids that have successfully ingested all objects at
 *   the end of batch processing.
 */
function hook_islandora_batch_process_finished($sets) {
  if (!empty($sets)) {
    module_load_include('inc', 'islandora_batch', 'includes/db');
    foreach ($sets as $set) {
      // Do something to the set, in this example it is completely deleting it
      // from the drupal database.
      islandora_batch_delete_set($set);
    }
  }
}

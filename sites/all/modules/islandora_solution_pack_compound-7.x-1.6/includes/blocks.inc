<?php

/**
 * @file
 * Holds block definitions for the compound solution pack.
 */

/**
 * Constructs the JAIL display block for a compound.
 *
 * @return array
 *   Renderable array for the block.
 */
function islandora_compound_object_jail_display_block() {
  $block = array();
  $object = menu_get_object('islandora_object', 2);
  if ($object) {
    // If only compound cmodel objects can be compound, this isn't necessary.
    $children = islandora_compound_object_get_parts($object);
    $pid = (!empty($children)) ? $children[0] : $object->id;
    $compound_object = islandora_object_load($pid);
    $compound_info = islandora_compound_object_retrieve_compound_info($compound_object);
    if ($compound_info) {
      $module_path = drupal_get_path('module', 'islandora_compound_object');
      $jail_path = libraries_get_path('JAIL');
      $block['#attached']['js'] = array(
        "$jail_path/lib/jquery.js" => array(
          'group' => JS_LIBRARY,
        ),
        "$jail_path/dist/jail.min.js" => array(
          'group' => JS_LIBRARY,
        ),
        "$module_path/js/compound_jail.js" => array(
          'group' => JS_LIBRARY,
        ),
      );
      $block['#attached']['css'] = array(
        "$module_path/css/islandora_compound_object.jail_block.css",
      );
      $block['part_title'] = array(
        '#type' => 'item',
        '#markup' => t('Part of: @label (@count @obj)', array(
          '@label' => $compound_info['parent_label'],
          '@count' => $compound_info['child_count'],
          '@obj' => format_plural($compound_info['child_count'], 'object', 'objects'),
        )),
      );
      if ($compound_info['parent_url']) {
        $block['manage_link'] = array(
          '#type' => 'item',
          '#markup' => l(t('manage parent'), $compound_info['parent_url']),
        );
      }
      foreach ($compound_info['siblings'] as $sibling) {
        $sibling_object = islandora_object_load($sibling);
        if (isset($sibling_object['TN']) && islandora_datastream_access(ISLANDORA_VIEW_OBJECTS, $sibling_object['TN'])) {
          $path = 'islandora/object/' . $sibling . '/datastream/TN/view';
        }
        else {
          // Object either does not have a thumbnail or it's restricted show a
          // default in its place.
          $image_path = drupal_get_path('module', 'islandora');
          $path = "$image_path/images/folder.png";
        }
        $class = array(
          'islandora-compound-object-jail',
        );
        if ($sibling == $pid) {
          $class[] = 'islandora-compound-object-jail-active';
        }
        $img = array(
          'image' => array(
            '#theme' => 'image',
            '#path' => "$module_path/images/loader.png",
            '#attributes' => array(
              'class' => $class,
              'data-src' => url($path),
            ),
            '#href' => "islandora/object/$sibling",
          ),
          'noscript' => array(
            '#theme' => 'image',
            '#path' => $path,
            '#prefix' => '<noscript>',
            '#suffix' => '</noscript>',
          ),
        );
        $block[$sibling] = array(
          '#type' => 'container',
          '#attributes' => array(),
          'link' => array(
            '#type' => 'link',
            '#title' => t("@title", array("@title" => $sibling_object->label)),
            '#href' => "islandora/object/$sibling",
          ),
          'content' => array(
            '#theme' => 'link',
            '#text' => drupal_render($img),
            '#path' => "islandora/object/$sibling",
            '#options' => array(
              'attributes' => array(),
              'html' => TRUE,
            ),
          ),
        );
      }
    }
  }
  return $block;
}

/**
 * Constructs the default navigation block for a compound.
 *
 * @param AbstractObject $object
 *   If supplied the object to create the block for.
 *
 * @return array
 *   Renderable array for the block.
 */
function islandora_compound_object_navigation_block(AbstractObject $object = NULL) {
  module_load_include('inc', 'islandora', 'includes/utilities');
  drupal_add_css(drupal_get_path('module', 'islandora_compound_object') . '/css/islandora_compound_object.block.css');
  $object = $object ? $object : menu_get_object('islandora_object', 2);
  if ($object) {
    // If only compound cmodel objects can be compound, this isn't necessary.
    $children = islandora_compound_object_get_parts($object->id);
    $pid = (!empty($children)) ? $children[0] : $object->id;
    $object = islandora_object_load($pid);
    $output = '';
    if ($object) {
      $compound_info = islandora_compound_object_retrieve_compound_info($object);
      // This object is part of a compound object.
      if (!empty($compound_info)) {
        $prev_next = theme('islandora_compound_prev_next', $compound_info);
        $output .= $prev_next;
      }

      if (!empty($output)) {
        return array('#markup' => $output);
      }
    }
  }
}

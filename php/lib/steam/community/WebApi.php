<?php
/**
 * This code is free software; you can redistribute it and/or modify it under
 * the terms of the new BSD License.
 *
 * Copyright (c) 2010, Sebastian Staudt
 *
 * @author     Sebastian Staudt
 * @license    http://www.opensource.org/licenses/bsd-license.php New BSD License
 * @package    Steam Condenser (PHP)
 * @subpackage Steam Community
 */

require_once 'exceptions/WebApiException.php';

/**
 * This adds support for Steam Web API to classes needing this functionality.
 * The Web API requires you to register a domain with your Steam account to
 * acquire an API key. See http://steamcommunity.com/dev for further details.
 *
 * @package Steam Condenser (PHP)
 * @subpackage Steam Community
 */
abstract class WebApi {

    private static $apiKey = null;

    /**
     * Returns the Steam Web API key
     */
    public static function getApiKey() {
        return self::$apiKey;
    }

    /**
     * Sets the Steam Web API key.
     */
    public static function setApiKey($apiKey) {
        if($apiKey != null && !preg_match('/^[0-9A-F]{32}$/', $apiKey)) {
            throw new WebApiException(WebApiException::INVALID_KEY);
        }

        self::$apiKey = $apiKey;
    }

    /**
     * Fetches JSON data from Steam Web API using the specified interface,
     * method and version. Additional parameters are supplied via HTTP GET.
     * Data is returned as a JSON-encoded string.
     */
    public static function getJSON($interface, $method, $version, $params = null) {
        return self::load('json', $interface, $method, $version, $params);
    }

    /**
     * Fetches JSON data from Steam Web API using the specified interface,
     * method and version. Additional parameters are supplied via HTTP GET.
     * Data is returned as a Hash containing the JSON data.
     */
    public static function getJSONData($interface, $method, $version, $params = null) {
        $data = self::getJSON($interface, $method, $version, $params);
        $result = json_decode($data)->result;

        if($result->status != 1) {
            throw new WebApiException(WebApiException::STATUS_BAD, $result->status, $result->statusDetail);
        }

        return $result;
    }

    /**
     * Fetches data from Steam Web API using the specified interface, method and
     * version. Additional parameters are supplied via HTTP GET.
     * Data is returned as a String in the given format (which may be 'json',
     * 'vdf', or 'xml').
     */
    public static function load($format, $interface, $method, $version, $params = null) {
        $version = str_pad($version, 4, '0', STR_PAD_LEFT);
        $url = "http://api.steampowered.com/$interface/$method/v$version/";

        $params['format'] = $format;
        $params['key']    = self::$apiKey;

        if($params != null && !empty($params)) {
            $url .= '?';
            foreach($params as $k => $v) {
                $url_params[] = "$k=$v";
            }
            $url .= join('&', $url_params);
        }


        $data = @file_get_contents($url);

        if(empty($data)) {
            preg_match('/^.* (\d{3}) (.*)$/', $http_response_header[0], $http_status);

            if($http_status[1] == 401) {
                throw new WebApiException(WebApiException::UNAUTHORIZED);
            }

            throw new WebApiException(WebApiException::HTTP_ERROR, $http_status[1], $http_status[2]);
        }

        return $data;
    }

}
?>